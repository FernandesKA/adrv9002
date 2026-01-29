# Устранение проблем стабильности ADRV9002

## Проблема: Сбой операции Prime Channel

### Симптомы
```
ARM Error: src=0x0, code=0xc909, action=-2
ARM Error: src=0x0, code=0x3, action=-2
Can't prime channel: -2
```

### Причина
ARM процессор чипа ADRV9002 периодически оказывается в несогласованном состоянии во время операций смены частоты. Это известная проблема ADI API, связанная с race condition между внутренними state machines чипа.

**Технические детали:**
- Ошибка 0xc909: сбой при переходе channel в состояние PRIMED
- Ошибка 0x3: `ADI_COMMON_ACT_ERR_CHECK_PARAM` - ARM считает параметры некорректными
- `action=-2`: требуется проверка параметров или сброс

## Реализованные исправления

### 1. Улучшенный retry механизм с экспоненциальным backoff
**Место:** `adrv9002_radio.c:405-470`

**Изменения:**
- Увеличено количество попыток: 1 → 2 (настраиваемо)
- Экспоненциальный backoff: 5ms, 10ms, 15ms...
- Более длительные проверки ARM health: 10ms → 50-100ms

**Настройка:**
```bash
# Изменить количество retry попыток (0-5)
echo 3 > /sys/module/adrv9002_drv/parameters/prime_retry_count

# По умолчанию: 2
```

### 2. Увеличенная задержка стабилизации после carrier configure
**Место:** `adrv9002_radio.c:370-385`

**Изменения:**
- Задержка после carrier config: 200-400us → 500-1000us
- Добавлена проверка ARM health перед carrier configure (20ms timeout)

**Обоснование:** ARM процессору нужно больше времени для обработки новой конфигурации PLL и синтезатора частот.

### 3. Автоматическая реинициализация при критических ошибках
**Место:** `adrv9002.c:364-376`

**Механизм:**
1. При неожиданных ошибках (кроме -EAGAIN, -EINVAL) триггерится auto-reinit
2. Выполняется полный hardware reset + переинициализация
3. Операция повторяется
4. Detailed logging отключается после первого reinit (снижение overhead)

**Настройка:**
```bash
# Включить/выключить auto-reinit
echo 1 > /sys/module/adrv9002_drv/parameters/auto_reinit  # включить (по умолчанию)
echo 0 > /sys/module/adrv9002_drv/parameters/auto_reinit  # выключить

# Мониторинг
cat /proc/adrv9002  # смотреть поле auto_reinit_count
```

## Рекомендуемые настройки

### Для стабильных систем (рекомендуется)
```bash
modprobe adrv9002_drv \
  auto_reinit=1 \
  prime_retry_count=2 \
  freq_change_timeout_us=200
```

### Для проблемных окружений (агрессивный режим)
```bash
modprobe adrv9002_drv \
  auto_reinit=1 \
  prime_retry_count=4 \
  freq_change_timeout_us=500 \
  verbose_logs=1
```

### Для разработки/отладки
```bash
modprobe adrv9002_drv \
  auto_reinit=1 \
  prime_retry_count=2 \
  detailed_logs=1 \
  verbose_logs=1 \
  measure_timing=1

# Смотреть логи
dmesg -w
cat /proc/adrv9002_log
```

## Мониторинг

### Проверка статуса
```bash
cat /proc/adrv9002
```

**Ключевые поля:**
- `initialized`: должно быть 1
- `auto_reinit_count`: количество выполненных auto-reinit (в идеале 0 или малое число)
- `lo_freq[0]`, `lo_freq[1]`: текущие частоты каналов

### Если проблемы сохраняются

1. **Увеличить prime_retry_count:**
   ```bash
   echo 4 > /sys/module/adrv9002_drv/parameters/prime_retry_count
   ```

2. **Увеличить базовый timeout:**
   ```bash
   echo 500 > /sys/module/adrv9002_drv/parameters/freq_change_timeout_us
   ```

3. **Включить подробное логирование:**
   ```bash
   echo 1 > /sys/module/adrv9002_drv/parameters/detailed_logs
   echo 1 > /sys/module/adrv9002_drv/parameters/verbose_logs
   cat /proc/adrv9002_log > /tmp/adrv9002_debug.log
   ```

## Ожидаемое поведение

### Нормальная работа
```
adrv9002 spi1.0: Frequency set ch0: 915000000 Hz (12345 us)
```

### Успешный retry
```
adrv9002 spi1.0: Can't prime channel: -2
adrv9002 spi1.0: Prime succeeded on retry 1 after initial error
adrv9002 spi1.0: Frequency set ch0: 915000000 Hz (23456 us)
```

### Успешный auto-reinit
```
adrv9002 spi1.0: Can't prime channel: -2
adrv9002 spi1.0: Prime retry 2 failed: -2
adrv9002 spi1.0: SET_FREQ failed with ret=-2, attempting auto-reinit
adrv9002 spi1.0: Attempting auto-reinit due to IOCTL error (count: 1)
adrv9002 spi1.0: Initialization complete
adrv9002 spi1.0: Auto-reinit successful
adrv9002 spi1.0: Auto-reinit succeeded, retrying SET_FREQ
adrv9002 spi1.0: Frequency set ch0: 915000000 Hz (7123456 us)
```

## Известные ограничения

1. **Auto-reinit время:** ~7 секунд (включая калибровку)
2. **Retry overhead:** каждый retry добавляет 5-15ms к времени смены частоты
3. **Detailed logging:** значительный overhead, отключается после первого reinit
4. **Clock reinit:** во время повторной инициализации пропадает clk lvds ssi, поэтому нужно заново перезаписать регистры PL

## Дополнительная информация

- ARM error codes: см. `module/common/adi_common_error_types.h`
- State machine diagram: ADI ADRV9001 System Development User Guide
- API reference: ADI ADRV9001 API Documentation
