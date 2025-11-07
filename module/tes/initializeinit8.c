#include "adi_adrv9001.h"
#include "adi_adrv9001_arm.h"
#include "adi_adrv9001_cals.h"
#include "adi_adrv9001_gpio.h"
#include "adi_adrv9001_mcs.h"
#include "adi_adrv9001_powermanagement.h"
#include "adi_adrv9001_powersavingandmonitormode.h"
#include "adi_adrv9001_radio.h"
#include "adi_adrv9001_rx.h"
#include "adi_adrv9001_ssi.h"
#include "adi_adrv9001_stream.h"
#include "adi_adrv9001_tx.h"

adi_adrv9001_Init_t initialize_init_8 = { 
		.clocks = { 
		.deviceClock_kHz = 300000, 
		.clkPllVcoFreq_daHz = 864000000, 
		.clkPllHsDiv = ADI_ADRV9001_HSDIV_4, 
		.clkPllMode = ADI_ADRV9001_CLK_PLL_HP_MODE, 
		.clk1105Div = ADI_ADRV9001_INTERNAL_CLOCK_DIV_2, 
		.armClkDiv = ADI_ADRV9001_INTERNAL_CLOCK_DIV_6, 
		.armPowerSavingClkDiv = 1, 
		.refClockOutEnable = true, 
		.auxPllPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH, 
		.clkPllPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH, 
		.padRefClkDrv = 0, 
		.extLo1OutFreq_kHz = 0, 
		.extLo2OutFreq_kHz = 0, 
		.rfPll1LoMode = ADI_ADRV9001_INT_LO1 | ADI_ADRV9001_INT_LO2, 
		.rfPll2LoMode = ADI_ADRV9001_INT_LO1 | ADI_ADRV9001_INT_LO2, 
		.ext1LoType = ADI_ADRV9001_EXT_LO_DIFFERENTIAL, 
		.ext2LoType = ADI_ADRV9001_EXT_LO_DIFFERENTIAL, 
		.rx1RfInputSel = ADI_ADRV9001_RX_A, 
		.rx2RfInputSel = ADI_ADRV9001_RX_A, 
		.extLo1Divider = 2, 
		.extLo2Divider = 2, 
		.rfPllPhaseSyncMode = ADI_ADRV9001_RFPLLMCS_NOSYNC, 
		.rx1LoSelect = ADI_ADRV9001_LOSEL_LO1, 
		.rx2LoSelect = ADI_ADRV9001_LOSEL_LO1, 
		.tx1LoSelect = ADI_ADRV9001_LOSEL_LO1, 
		.tx2LoSelect = ADI_ADRV9001_LOSEL_LO1, 
		.rx1LoDivMode = ADI_ADRV9001_LO_DIV_MODE_LOW_POWER, 
		.rx2LoDivMode = ADI_ADRV9001_LO_DIV_MODE_LOW_POWER, 
		.tx1LoDivMode = ADI_ADRV9001_LO_DIV_MODE_LOW_POWER, 
		.tx2LoDivMode = ADI_ADRV9001_LO_DIV_MODE_LOW_POWER, 
		.loGen1Select = ADI_ADRV9001_LOGENPOWER_RFPLL_LDO, 
		.loGen2Select = ADI_ADRV9001_LOGENPOWER_RFPLL_LDO }
	, 
		.rx = { 
		.rxInitChannelMask = 195, 
		.rxChannelCfg = { { 
		.profile = { 
		.primarySigBandwidth_Hz = 39000000, 
		.rxOutputRate_Hz = 60000000, 
		.rxInterfaceSampleRate_Hz = 60000000, 
		.rxOffsetLo_kHz = 0, 
		.rxNcoEnable = true, 
		.outputSignaling = ADI_ADRV9001_RX_IQ, 
		.filterOrder = 1, 
		.filterOrderLp = 1, 
		.hpAdcCorner = 20000000, 
		.lpAdcCorner = 0, 
		.adcClk_kHz = 2160000, 
		.rxCorner3dB_kHz = 40000, 
		.rxCorner3dBLp_kHz = 40000, 
		.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH, 
		.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH, 
		.channelType = 1, 
		.adcType = ADI_ADRV9001_ADC_HP, 
		.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC, 
		.gainTableType = ADI_ADRV9001_RX_GAIN_CORRECTION_TABLE, 
		.rxDpProfile = { 
		.rxNbDecTop = { 
		.scicBlk23En = 0, 
		.scicBlk23DivFactor = 1, 
		.scicBlk23LowRippleEn = 0, 
		.decBy2Blk35En = 0, 
		.decBy2Blk37En = 0, 
		.decBy2Blk39En = 0, 
		.decBy2Blk41En = 0, 
		.decBy2Blk43En = 0, 
		.decBy3Blk45En = 0, 
		.decBy2Blk47En = 0 }
					, 
		.rxWbDecTop = { 
		.decBy2Blk25En = 0, 
		.decBy2Blk27En = 0, 
		.decBy2Blk29En = 0, 
		.decBy2Blk31En = 0, 
		.decBy2Blk33En = 0, 
		.wbLpfBlk33p1En = 0 }
					, 
		.rxDecTop = { 
		.decBy3Blk15En = 1, 
		.decBy2Hb3Blk17p1En = 0, 
		.decBy2Hb4Blk17p2En = 0, 
		.decBy2Hb5Blk19p1En = 0, 
		.decBy2Hb6Blk19p2En = 0 }
					, 
		.rxSincHBTop = { 
		.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB, 
		.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_SINC6, 
		.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB1, 
		.isGainCompEnabled = 0, 
		.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384  }, 
		.gainComp9GainQ = { 0, 0, 0, 0, 0, 0  } }
					, 
		.rxNbDem = { 
		.dpInFifo = { 
		.dpInFifoEn = 0, 
		.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING, 
		.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL }
						, 
		.rxNbNco = { 
		.rxNbNcoEn = 1, 
		.rxNbNcoConfig = { 
		.freq = 0, 
		.sampleFreq = 60000000, 
		.phase = 0, 
		.realOut = 0 }
							 }
						, 
		.rxWbNbCompPFir = { 
		.bankSel = ADI_ADRV9001_PFIR_BANK_A, 
		.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 0 }
						, 
		.resamp = { 
		.rxResampEn = 0, 
		.resampPhaseI = 0, 
		.resampPhaseQ = 0 }
						, 
		.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS, 
		.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL, 
		.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ, 
		.dpArmSel = ADI_ADRV9001_DP_SEL }
					 }
				, 
		.lnaConfig = { 
		.externalLnaPresent = false, 
		.gpioSourceSel = ADI_ADRV9001_GPIO_ANALOG_PIN_NIBBLE_UNASSIGNED, 
		.externalLnaPinSel = ADI_ADRV9001_EXTERNAL_LNA_PIN_RX1_LOWER_RX2_UPPER, 
		.settlingDelay = 0, 
		.numberLnaGainSteps = 0, 
		.lnaGainSteps_mdB = { 0, 0, 0, 0  }, 
		.lnaDigitalGainDelay = 0, 
		.minGainIndex = 0, 
		.lnaType = ADI_ADRV9001_EXTERNAL_LNA_TYPE_SINGLE }
				, 
		.rxSsiConfig = { 
		.ssiType = ADI_ADRV9001_SSI_TYPE_LVDS, 
		.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA, 
		.numLaneSel = ADI_ADRV9001_SSI_2_LANE, 
		.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsIBitInversion = false, 
		.lvdsQBitInversion = false, 
		.lvdsStrobeBitInversion = false, 
		.lvdsUseLsbIn12bitMode = 0, 
		.lvdsRxClkInversionEn = false, 
		.cmosDdrPosClkEn = false, 
		.cmosClkInversionEn = false, 
		.ddrEn = true, 
		.rxMaskStrobeEn = false }
				 }
			 }, { 
		.profile = { 
		.primarySigBandwidth_Hz = 39000000, 
		.rxOutputRate_Hz = 60000000, 
		.rxInterfaceSampleRate_Hz = 60000000, 
		.rxOffsetLo_kHz = 0, 
		.rxNcoEnable = true, 
		.outputSignaling = ADI_ADRV9001_RX_IQ, 
		.filterOrder = 1, 
		.filterOrderLp = 1, 
		.hpAdcCorner = 20000000, 
		.lpAdcCorner = 0, 
		.adcClk_kHz = 2160000, 
		.rxCorner3dB_kHz = 40000, 
		.rxCorner3dBLp_kHz = 40000, 
		.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH, 
		.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH, 
		.channelType = 2, 
		.adcType = ADI_ADRV9001_ADC_HP, 
		.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC, 
		.gainTableType = ADI_ADRV9001_RX_GAIN_CORRECTION_TABLE, 
		.rxDpProfile = { 
		.rxNbDecTop = { 
		.scicBlk23En = 0, 
		.scicBlk23DivFactor = 1, 
		.scicBlk23LowRippleEn = 0, 
		.decBy2Blk35En = 0, 
		.decBy2Blk37En = 0, 
		.decBy2Blk39En = 0, 
		.decBy2Blk41En = 0, 
		.decBy2Blk43En = 0, 
		.decBy3Blk45En = 0, 
		.decBy2Blk47En = 0 }
					, 
		.rxWbDecTop = { 
		.decBy2Blk25En = 0, 
		.decBy2Blk27En = 0, 
		.decBy2Blk29En = 0, 
		.decBy2Blk31En = 0, 
		.decBy2Blk33En = 0, 
		.wbLpfBlk33p1En = 0 }
					, 
		.rxDecTop = { 
		.decBy3Blk15En = 1, 
		.decBy2Hb3Blk17p1En = 0, 
		.decBy2Hb4Blk17p2En = 0, 
		.decBy2Hb5Blk19p1En = 0, 
		.decBy2Hb6Blk19p2En = 0 }
					, 
		.rxSincHBTop = { 
		.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB, 
		.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_SINC6, 
		.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB1, 
		.isGainCompEnabled = 0, 
		.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384  }, 
		.gainComp9GainQ = { 0, 0, 0, 0, 0, 0  } }
					, 
		.rxNbDem = { 
		.dpInFifo = { 
		.dpInFifoEn = 0, 
		.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING, 
		.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL }
						, 
		.rxNbNco = { 
		.rxNbNcoEn = 1, 
		.rxNbNcoConfig = { 
		.freq = 0, 
		.sampleFreq = 60000000, 
		.phase = 0, 
		.realOut = 0 }
							 }
						, 
		.rxWbNbCompPFir = { 
		.bankSel = ADI_ADRV9001_PFIR_BANK_C, 
		.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 0 }
						, 
		.resamp = { 
		.rxResampEn = 0, 
		.resampPhaseI = 0, 
		.resampPhaseQ = 0 }
						, 
		.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS, 
		.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL, 
		.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ, 
		.dpArmSel = ADI_ADRV9001_DP_SEL }
					 }
				, 
		.lnaConfig = { 
		.externalLnaPresent = false, 
		.gpioSourceSel = ADI_ADRV9001_GPIO_ANALOG_PIN_NIBBLE_UNASSIGNED, 
		.externalLnaPinSel = ADI_ADRV9001_EXTERNAL_LNA_PIN_RX1_LOWER_RX2_UPPER, 
		.settlingDelay = 0, 
		.numberLnaGainSteps = 0, 
		.lnaGainSteps_mdB = { 0, 0, 0, 0  }, 
		.lnaDigitalGainDelay = 0, 
		.minGainIndex = 0, 
		.lnaType = ADI_ADRV9001_EXTERNAL_LNA_TYPE_SINGLE }
				, 
		.rxSsiConfig = { 
		.ssiType = ADI_ADRV9001_SSI_TYPE_LVDS, 
		.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA, 
		.numLaneSel = ADI_ADRV9001_SSI_2_LANE, 
		.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsIBitInversion = false, 
		.lvdsQBitInversion = false, 
		.lvdsStrobeBitInversion = false, 
		.lvdsUseLsbIn12bitMode = 0, 
		.lvdsRxClkInversionEn = false, 
		.cmosDdrPosClkEn = false, 
		.cmosClkInversionEn = false, 
		.ddrEn = true, 
		.rxMaskStrobeEn = false }
				 }
			 }, { 
		.profile = { 
		.primarySigBandwidth_Hz = 12500, 
		.rxOutputRate_Hz = 0, 
		.rxInterfaceSampleRate_Hz = 0, 
		.rxOffsetLo_kHz = 0, 
		.rxNcoEnable = false, 
		.outputSignaling = ADI_ADRV9001_RX_IQ, 
		.filterOrder = 1, 
		.filterOrderLp = 1, 
		.hpAdcCorner = 0, 
		.lpAdcCorner = 0, 
		.adcClk_kHz = 0, 
		.rxCorner3dB_kHz = 0, 
		.rxCorner3dBLp_kHz = 0, 
		.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH, 
		.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH, 
		.channelType = 0, 
		.adcType = ADI_ADRV9001_ADC_HP, 
		.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC, 
		.gainTableType = ADI_ADRV9001_RX_GAIN_CORRECTION_TABLE, 
		.rxDpProfile = { 
		.rxNbDecTop = { 
		.scicBlk23En = 0, 
		.scicBlk23DivFactor = 0, 
		.scicBlk23LowRippleEn = 0, 
		.decBy2Blk35En = 0, 
		.decBy2Blk37En = 0, 
		.decBy2Blk39En = 0, 
		.decBy2Blk41En = 0, 
		.decBy2Blk43En = 0, 
		.decBy3Blk45En = 0, 
		.decBy2Blk47En = 0 }
					, 
		.rxWbDecTop = { 
		.decBy2Blk25En = 0, 
		.decBy2Blk27En = 0, 
		.decBy2Blk29En = 0, 
		.decBy2Blk31En = 0, 
		.decBy2Blk33En = 0, 
		.wbLpfBlk33p1En = 0 }
					, 
		.rxDecTop = { 
		.decBy3Blk15En = 0, 
		.decBy2Hb3Blk17p1En = 0, 
		.decBy2Hb4Blk17p2En = 0, 
		.decBy2Hb5Blk19p1En = 0, 
		.decBy2Hb6Blk19p2En = 0 }
					, 
		.rxSincHBTop = { 
		.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB, 
		.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_ZERO, 
		.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB1, 
		.isGainCompEnabled = 0, 
		.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384  }, 
		.gainComp9GainQ = { 0, 0, 0, 0, 0, 0  } }
					, 
		.rxNbDem = { 
		.dpInFifo = { 
		.dpInFifoEn = 0, 
		.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING, 
		.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL }
						, 
		.rxNbNco = { 
		.rxNbNcoEn = 0, 
		.rxNbNcoConfig = { 
		.freq = 0, 
		.sampleFreq = 0, 
		.phase = 0, 
		.realOut = 0 }
							 }
						, 
		.rxWbNbCompPFir = { 
		.bankSel = ADI_ADRV9001_PFIR_BANK_A, 
		.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 0 }
						, 
		.resamp = { 
		.rxResampEn = 0, 
		.resampPhaseI = 0, 
		.resampPhaseQ = 0 }
						, 
		.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS, 
		.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL, 
		.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ, 
		.dpArmSel = ADI_ADRV9001_DP_SEL }
					 }
				, 
		.lnaConfig = { 
		.externalLnaPresent = false, 
		.gpioSourceSel = ADI_ADRV9001_GPIO_ANALOG_PIN_NIBBLE_UNASSIGNED, 
		.externalLnaPinSel = ADI_ADRV9001_EXTERNAL_LNA_PIN_RX1_LOWER_RX2_UPPER, 
		.settlingDelay = 0, 
		.numberLnaGainSteps = 0, 
		.lnaGainSteps_mdB = { 0, 0, 0, 0  }, 
		.lnaDigitalGainDelay = 0, 
		.minGainIndex = 0, 
		.lnaType = ADI_ADRV9001_EXTERNAL_LNA_TYPE_SINGLE }
				, 
		.rxSsiConfig = { 
		.ssiType = ADI_ADRV9001_SSI_TYPE_DISABLE, 
		.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA, 
		.numLaneSel = ADI_ADRV9001_SSI_1_LANE, 
		.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsIBitInversion = false, 
		.lvdsQBitInversion = false, 
		.lvdsStrobeBitInversion = false, 
		.lvdsUseLsbIn12bitMode = 0, 
		.lvdsRxClkInversionEn = false, 
		.cmosDdrPosClkEn = false, 
		.cmosClkInversionEn = false, 
		.ddrEn = false, 
		.rxMaskStrobeEn = false }
				 }
			 }, { 
		.profile = { 
		.primarySigBandwidth_Hz = 12500, 
		.rxOutputRate_Hz = 0, 
		.rxInterfaceSampleRate_Hz = 0, 
		.rxOffsetLo_kHz = 0, 
		.rxNcoEnable = false, 
		.outputSignaling = ADI_ADRV9001_RX_IQ, 
		.filterOrder = 1, 
		.filterOrderLp = 1, 
		.hpAdcCorner = 0, 
		.lpAdcCorner = 0, 
		.adcClk_kHz = 0, 
		.rxCorner3dB_kHz = 0, 
		.rxCorner3dBLp_kHz = 0, 
		.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH, 
		.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH, 
		.channelType = 0, 
		.adcType = ADI_ADRV9001_ADC_HP, 
		.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC, 
		.gainTableType = ADI_ADRV9001_RX_GAIN_CORRECTION_TABLE, 
		.rxDpProfile = { 
		.rxNbDecTop = { 
		.scicBlk23En = 0, 
		.scicBlk23DivFactor = 0, 
		.scicBlk23LowRippleEn = 0, 
		.decBy2Blk35En = 0, 
		.decBy2Blk37En = 0, 
		.decBy2Blk39En = 0, 
		.decBy2Blk41En = 0, 
		.decBy2Blk43En = 0, 
		.decBy3Blk45En = 0, 
		.decBy2Blk47En = 0 }
					, 
		.rxWbDecTop = { 
		.decBy2Blk25En = 0, 
		.decBy2Blk27En = 0, 
		.decBy2Blk29En = 0, 
		.decBy2Blk31En = 0, 
		.decBy2Blk33En = 0, 
		.wbLpfBlk33p1En = 0 }
					, 
		.rxDecTop = { 
		.decBy3Blk15En = 0, 
		.decBy2Hb3Blk17p1En = 0, 
		.decBy2Hb4Blk17p2En = 0, 
		.decBy2Hb5Blk19p1En = 0, 
		.decBy2Hb6Blk19p2En = 0 }
					, 
		.rxSincHBTop = { 
		.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB, 
		.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_ZERO, 
		.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB1, 
		.isGainCompEnabled = 0, 
		.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384  }, 
		.gainComp9GainQ = { 0, 0, 0, 0, 0, 0  } }
					, 
		.rxNbDem = { 
		.dpInFifo = { 
		.dpInFifoEn = 0, 
		.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING, 
		.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL }
						, 
		.rxNbNco = { 
		.rxNbNcoEn = 0, 
		.rxNbNcoConfig = { 
		.freq = 0, 
		.sampleFreq = 0, 
		.phase = 0, 
		.realOut = 0 }
							 }
						, 
		.rxWbNbCompPFir = { 
		.bankSel = ADI_ADRV9001_PFIR_BANK_A, 
		.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 0 }
						, 
		.resamp = { 
		.rxResampEn = 0, 
		.resampPhaseI = 0, 
		.resampPhaseQ = 0 }
						, 
		.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS, 
		.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL, 
		.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ, 
		.dpArmSel = ADI_ADRV9001_DP_SEL }
					 }
				, 
		.lnaConfig = { 
		.externalLnaPresent = false, 
		.gpioSourceSel = ADI_ADRV9001_GPIO_ANALOG_PIN_NIBBLE_UNASSIGNED, 
		.externalLnaPinSel = ADI_ADRV9001_EXTERNAL_LNA_PIN_RX1_LOWER_RX2_UPPER, 
		.settlingDelay = 0, 
		.numberLnaGainSteps = 0, 
		.lnaGainSteps_mdB = { 0, 0, 0, 0  }, 
		.lnaDigitalGainDelay = 0, 
		.minGainIndex = 0, 
		.lnaType = ADI_ADRV9001_EXTERNAL_LNA_TYPE_SINGLE }
				, 
		.rxSsiConfig = { 
		.ssiType = ADI_ADRV9001_SSI_TYPE_DISABLE, 
		.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA, 
		.numLaneSel = ADI_ADRV9001_SSI_1_LANE, 
		.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsIBitInversion = false, 
		.lvdsQBitInversion = false, 
		.lvdsStrobeBitInversion = false, 
		.lvdsUseLsbIn12bitMode = 0, 
		.lvdsRxClkInversionEn = false, 
		.cmosDdrPosClkEn = false, 
		.cmosClkInversionEn = false, 
		.ddrEn = false, 
		.rxMaskStrobeEn = false }
				 }
			 }, { 
		.profile = { 
		.primarySigBandwidth_Hz = 30000000, 
		.rxOutputRate_Hz = 60000000, 
		.rxInterfaceSampleRate_Hz = 60000000, 
		.rxOffsetLo_kHz = 0, 
		.rxNcoEnable = false, 
		.outputSignaling = ADI_ADRV9001_RX_IQ, 
		.filterOrder = 1, 
		.filterOrderLp = 1, 
		.hpAdcCorner = 50000000, 
		.lpAdcCorner = 0, 
		.adcClk_kHz = 2160000, 
		.rxCorner3dB_kHz = 100000, 
		.rxCorner3dBLp_kHz = 100000, 
		.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH, 
		.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH, 
		.channelType = 64, 
		.adcType = ADI_ADRV9001_ADC_HP, 
		.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC, 
		.gainTableType = ADI_ADRV9001_RX_GAIN_CORRECTION_TABLE, 
		.rxDpProfile = { 
		.rxNbDecTop = { 
		.scicBlk23En = 0, 
		.scicBlk23DivFactor = 1, 
		.scicBlk23LowRippleEn = 0, 
		.decBy2Blk35En = 0, 
		.decBy2Blk37En = 0, 
		.decBy2Blk39En = 0, 
		.decBy2Blk41En = 0, 
		.decBy2Blk43En = 0, 
		.decBy3Blk45En = 0, 
		.decBy2Blk47En = 0 }
					, 
		.rxWbDecTop = { 
		.decBy2Blk25En = 0, 
		.decBy2Blk27En = 0, 
		.decBy2Blk29En = 0, 
		.decBy2Blk31En = 0, 
		.decBy2Blk33En = 0, 
		.wbLpfBlk33p1En = 0 }
					, 
		.rxDecTop = { 
		.decBy3Blk15En = 1, 
		.decBy2Hb3Blk17p1En = 0, 
		.decBy2Hb4Blk17p2En = 0, 
		.decBy2Hb5Blk19p1En = 0, 
		.decBy2Hb6Blk19p2En = 0 }
					, 
		.rxSincHBTop = { 
		.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB, 
		.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_SINC6, 
		.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB2, 
		.isGainCompEnabled = 0, 
		.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384  }, 
		.gainComp9GainQ = { 0, 0, 0, 0, 0, 0  } }
					, 
		.rxNbDem = { 
		.dpInFifo = { 
		.dpInFifoEn = 0, 
		.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING, 
		.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL }
						, 
		.rxNbNco = { 
		.rxNbNcoEn = 0, 
		.rxNbNcoConfig = { 
		.freq = 0, 
		.sampleFreq = 0, 
		.phase = 0, 
		.realOut = 0 }
							 }
						, 
		.rxWbNbCompPFir = { 
		.bankSel = ADI_ADRV9001_PFIR_BANK_B, 
		.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 0 }
						, 
		.resamp = { 
		.rxResampEn = 0, 
		.resampPhaseI = 0, 
		.resampPhaseQ = 0 }
						, 
		.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS, 
		.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL, 
		.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ, 
		.dpArmSel = ADI_ADRV9001_DP_SEL }
					 }
				, 
		.lnaConfig = { 
		.externalLnaPresent = false, 
		.gpioSourceSel = ADI_ADRV9001_GPIO_ANALOG_PIN_NIBBLE_UNASSIGNED, 
		.externalLnaPinSel = ADI_ADRV9001_EXTERNAL_LNA_PIN_RX1_LOWER_RX2_UPPER, 
		.settlingDelay = 0, 
		.numberLnaGainSteps = 0, 
		.lnaGainSteps_mdB = { 0, 0, 0, 0  }, 
		.lnaDigitalGainDelay = 0, 
		.minGainIndex = 0, 
		.lnaType = ADI_ADRV9001_EXTERNAL_LNA_TYPE_SINGLE }
				, 
		.rxSsiConfig = { 
		.ssiType = ADI_ADRV9001_SSI_TYPE_LVDS, 
		.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA, 
		.numLaneSel = ADI_ADRV9001_SSI_2_LANE, 
		.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsIBitInversion = false, 
		.lvdsQBitInversion = false, 
		.lvdsStrobeBitInversion = false, 
		.lvdsUseLsbIn12bitMode = 0, 
		.lvdsRxClkInversionEn = false, 
		.cmosDdrPosClkEn = false, 
		.cmosClkInversionEn = false, 
		.ddrEn = true, 
		.rxMaskStrobeEn = false }
				 }
			 }, { 
		.profile = { 
		.primarySigBandwidth_Hz = 30000000, 
		.rxOutputRate_Hz = 60000000, 
		.rxInterfaceSampleRate_Hz = 60000000, 
		.rxOffsetLo_kHz = 0, 
		.rxNcoEnable = false, 
		.outputSignaling = ADI_ADRV9001_RX_IQ, 
		.filterOrder = 1, 
		.filterOrderLp = 1, 
		.hpAdcCorner = 50000000, 
		.lpAdcCorner = 0, 
		.adcClk_kHz = 2160000, 
		.rxCorner3dB_kHz = 100000, 
		.rxCorner3dBLp_kHz = 100000, 
		.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH, 
		.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH, 
		.channelType = 128, 
		.adcType = ADI_ADRV9001_ADC_HP, 
		.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC, 
		.gainTableType = ADI_ADRV9001_RX_GAIN_CORRECTION_TABLE, 
		.rxDpProfile = { 
		.rxNbDecTop = { 
		.scicBlk23En = 0, 
		.scicBlk23DivFactor = 1, 
		.scicBlk23LowRippleEn = 0, 
		.decBy2Blk35En = 0, 
		.decBy2Blk37En = 0, 
		.decBy2Blk39En = 0, 
		.decBy2Blk41En = 0, 
		.decBy2Blk43En = 0, 
		.decBy3Blk45En = 0, 
		.decBy2Blk47En = 0 }
					, 
		.rxWbDecTop = { 
		.decBy2Blk25En = 0, 
		.decBy2Blk27En = 0, 
		.decBy2Blk29En = 0, 
		.decBy2Blk31En = 0, 
		.decBy2Blk33En = 0, 
		.wbLpfBlk33p1En = 0 }
					, 
		.rxDecTop = { 
		.decBy3Blk15En = 1, 
		.decBy2Hb3Blk17p1En = 0, 
		.decBy2Hb4Blk17p2En = 0, 
		.decBy2Hb5Blk19p1En = 0, 
		.decBy2Hb6Blk19p2En = 0 }
					, 
		.rxSincHBTop = { 
		.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB, 
		.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_SINC6, 
		.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB2, 
		.isGainCompEnabled = 0, 
		.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384  }, 
		.gainComp9GainQ = { 0, 0, 0, 0, 0, 0  } }
					, 
		.rxNbDem = { 
		.dpInFifo = { 
		.dpInFifoEn = 0, 
		.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING, 
		.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL }
						, 
		.rxNbNco = { 
		.rxNbNcoEn = 0, 
		.rxNbNcoConfig = { 
		.freq = 0, 
		.sampleFreq = 0, 
		.phase = 0, 
		.realOut = 0 }
							 }
						, 
		.rxWbNbCompPFir = { 
		.bankSel = ADI_ADRV9001_PFIR_BANK_D, 
		.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 0 }
						, 
		.resamp = { 
		.rxResampEn = 0, 
		.resampPhaseI = 0, 
		.resampPhaseQ = 0 }
						, 
		.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS, 
		.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL, 
		.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ, 
		.dpArmSel = ADI_ADRV9001_DP_SEL }
					 }
				, 
		.lnaConfig = { 
		.externalLnaPresent = false, 
		.gpioSourceSel = ADI_ADRV9001_GPIO_ANALOG_PIN_NIBBLE_UNASSIGNED, 
		.externalLnaPinSel = ADI_ADRV9001_EXTERNAL_LNA_PIN_RX1_LOWER_RX2_UPPER, 
		.settlingDelay = 0, 
		.numberLnaGainSteps = 0, 
		.lnaGainSteps_mdB = { 0, 0, 0, 0  }, 
		.lnaDigitalGainDelay = 0, 
		.minGainIndex = 0, 
		.lnaType = ADI_ADRV9001_EXTERNAL_LNA_TYPE_SINGLE }
				, 
		.rxSsiConfig = { 
		.ssiType = ADI_ADRV9001_SSI_TYPE_LVDS, 
		.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA, 
		.numLaneSel = ADI_ADRV9001_SSI_2_LANE, 
		.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsIBitInversion = false, 
		.lvdsQBitInversion = false, 
		.lvdsStrobeBitInversion = false, 
		.lvdsUseLsbIn12bitMode = 0, 
		.lvdsRxClkInversionEn = false, 
		.cmosDdrPosClkEn = false, 
		.cmosClkInversionEn = false, 
		.ddrEn = true, 
		.rxMaskStrobeEn = false }
				 }
			 }, { 
		.profile = { 
		.primarySigBandwidth_Hz = 12500, 
		.rxOutputRate_Hz = 0, 
		.rxInterfaceSampleRate_Hz = 0, 
		.rxOffsetLo_kHz = 0, 
		.rxNcoEnable = false, 
		.outputSignaling = ADI_ADRV9001_RX_IQ, 
		.filterOrder = 1, 
		.filterOrderLp = 1, 
		.hpAdcCorner = 0, 
		.lpAdcCorner = 0, 
		.adcClk_kHz = 0, 
		.rxCorner3dB_kHz = 0, 
		.rxCorner3dBLp_kHz = 0, 
		.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH, 
		.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH, 
		.channelType = 0, 
		.adcType = ADI_ADRV9001_ADC_HP, 
		.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC, 
		.gainTableType = ADI_ADRV9001_RX_GAIN_CORRECTION_TABLE, 
		.rxDpProfile = { 
		.rxNbDecTop = { 
		.scicBlk23En = 0, 
		.scicBlk23DivFactor = 0, 
		.scicBlk23LowRippleEn = 0, 
		.decBy2Blk35En = 0, 
		.decBy2Blk37En = 0, 
		.decBy2Blk39En = 0, 
		.decBy2Blk41En = 0, 
		.decBy2Blk43En = 0, 
		.decBy3Blk45En = 0, 
		.decBy2Blk47En = 0 }
					, 
		.rxWbDecTop = { 
		.decBy2Blk25En = 0, 
		.decBy2Blk27En = 0, 
		.decBy2Blk29En = 0, 
		.decBy2Blk31En = 0, 
		.decBy2Blk33En = 0, 
		.wbLpfBlk33p1En = 0 }
					, 
		.rxDecTop = { 
		.decBy3Blk15En = 0, 
		.decBy2Hb3Blk17p1En = 0, 
		.decBy2Hb4Blk17p2En = 0, 
		.decBy2Hb5Blk19p1En = 0, 
		.decBy2Hb6Blk19p2En = 0 }
					, 
		.rxSincHBTop = { 
		.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB, 
		.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_ZERO, 
		.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB1, 
		.isGainCompEnabled = 0, 
		.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384  }, 
		.gainComp9GainQ = { 0, 0, 0, 0, 0, 0  } }
					, 
		.rxNbDem = { 
		.dpInFifo = { 
		.dpInFifoEn = 0, 
		.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING, 
		.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL }
						, 
		.rxNbNco = { 
		.rxNbNcoEn = 0, 
		.rxNbNcoConfig = { 
		.freq = 0, 
		.sampleFreq = 0, 
		.phase = 0, 
		.realOut = 0 }
							 }
						, 
		.rxWbNbCompPFir = { 
		.bankSel = ADI_ADRV9001_PFIR_BANK_A, 
		.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 0 }
						, 
		.resamp = { 
		.rxResampEn = 0, 
		.resampPhaseI = 0, 
		.resampPhaseQ = 0 }
						, 
		.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS, 
		.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL, 
		.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ, 
		.dpArmSel = ADI_ADRV9001_DP_SEL }
					 }
				, 
		.lnaConfig = { 
		.externalLnaPresent = false, 
		.gpioSourceSel = ADI_ADRV9001_GPIO_ANALOG_PIN_NIBBLE_UNASSIGNED, 
		.externalLnaPinSel = ADI_ADRV9001_EXTERNAL_LNA_PIN_RX1_LOWER_RX2_UPPER, 
		.settlingDelay = 0, 
		.numberLnaGainSteps = 0, 
		.lnaGainSteps_mdB = { 0, 0, 0, 0  }, 
		.lnaDigitalGainDelay = 0, 
		.minGainIndex = 0, 
		.lnaType = ADI_ADRV9001_EXTERNAL_LNA_TYPE_SINGLE }
				, 
		.rxSsiConfig = { 
		.ssiType = ADI_ADRV9001_SSI_TYPE_DISABLE, 
		.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA, 
		.numLaneSel = ADI_ADRV9001_SSI_1_LANE, 
		.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsIBitInversion = false, 
		.lvdsQBitInversion = false, 
		.lvdsStrobeBitInversion = false, 
		.lvdsUseLsbIn12bitMode = 0, 
		.lvdsRxClkInversionEn = false, 
		.cmosDdrPosClkEn = false, 
		.cmosClkInversionEn = false, 
		.ddrEn = false, 
		.rxMaskStrobeEn = false }
				 }
			 }, { 
		.profile = { 
		.primarySigBandwidth_Hz = 12500, 
		.rxOutputRate_Hz = 0, 
		.rxInterfaceSampleRate_Hz = 0, 
		.rxOffsetLo_kHz = 0, 
		.rxNcoEnable = false, 
		.outputSignaling = ADI_ADRV9001_RX_IQ, 
		.filterOrder = 1, 
		.filterOrderLp = 1, 
		.hpAdcCorner = 0, 
		.lpAdcCorner = 0, 
		.adcClk_kHz = 0, 
		.rxCorner3dB_kHz = 0, 
		.rxCorner3dBLp_kHz = 0, 
		.tiaPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH, 
		.tiaPowerLp = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH, 
		.channelType = 0, 
		.adcType = ADI_ADRV9001_ADC_HP, 
		.lpAdcCalMode = ADI_ADRV9001_ADC_LOWPOWER_PERIODIC, 
		.gainTableType = ADI_ADRV9001_RX_GAIN_CORRECTION_TABLE, 
		.rxDpProfile = { 
		.rxNbDecTop = { 
		.scicBlk23En = 0, 
		.scicBlk23DivFactor = 0, 
		.scicBlk23LowRippleEn = 0, 
		.decBy2Blk35En = 0, 
		.decBy2Blk37En = 0, 
		.decBy2Blk39En = 0, 
		.decBy2Blk41En = 0, 
		.decBy2Blk43En = 0, 
		.decBy3Blk45En = 0, 
		.decBy2Blk47En = 0 }
					, 
		.rxWbDecTop = { 
		.decBy2Blk25En = 0, 
		.decBy2Blk27En = 0, 
		.decBy2Blk29En = 0, 
		.decBy2Blk31En = 0, 
		.decBy2Blk33En = 0, 
		.wbLpfBlk33p1En = 0 }
					, 
		.rxDecTop = { 
		.decBy3Blk15En = 0, 
		.decBy2Hb3Blk17p1En = 0, 
		.decBy2Hb4Blk17p2En = 0, 
		.decBy2Hb5Blk19p1En = 0, 
		.decBy2Hb6Blk19p2En = 0 }
					, 
		.rxSincHBTop = { 
		.sincGainMux = ADI_ADRV9001_RX_SINC_GAIN_MUX_6_DB, 
		.sincMux = ADI_ADRV9001_RX_SINC_MUX5_OUTPUT_ZERO, 
		.hbMux = ADI_ADRV9001_RX_HB_MUX_OUTPUT_HB1, 
		.isGainCompEnabled = 0, 
		.gainComp9GainI = { 16384, 16384, 16384, 16384, 16384, 16384  }, 
		.gainComp9GainQ = { 0, 0, 0, 0, 0, 0  } }
					, 
		.rxNbDem = { 
		.dpInFifo = { 
		.dpInFifoEn = 0, 
		.dpInFifoMode = ADI_ADRV9001_DP_IN_FIFO_MODE_DETECTING, 
		.dpInFifoTestDataSel = ADI_ADRV9001_DP_IN_FIFO_INPUT_DP_SEL }
						, 
		.rxNbNco = { 
		.rxNbNcoEn = 0, 
		.rxNbNcoConfig = { 
		.freq = 0, 
		.sampleFreq = 0, 
		.phase = 0, 
		.realOut = 0 }
							 }
						, 
		.rxWbNbCompPFir = { 
		.bankSel = ADI_ADRV9001_PFIR_BANK_A, 
		.rxWbNbCompPFirInMuxSel = ADI_ADRV9001_RP_FIR_IN_MUX_INT_IN, 
		.rxWbNbCompPFirEn = 0 }
						, 
		.resamp = { 
		.rxResampEn = 0, 
		.resampPhaseI = 0, 
		.resampPhaseQ = 0 }
						, 
		.gsOutMuxSel = ADI_ADRV9001_GS_OUT_MUX_BYPASS, 
		.rxOutSel = ADI_ADRV9001_RX_OUT_IQ_SEL, 
		.rxRoundMode = ADI_ADRV9001_RX_ROUNDMODE_IQ, 
		.dpArmSel = ADI_ADRV9001_DP_SEL }
					 }
				, 
		.lnaConfig = { 
		.externalLnaPresent = false, 
		.gpioSourceSel = ADI_ADRV9001_GPIO_ANALOG_PIN_NIBBLE_UNASSIGNED, 
		.externalLnaPinSel = ADI_ADRV9001_EXTERNAL_LNA_PIN_RX1_LOWER_RX2_UPPER, 
		.settlingDelay = 0, 
		.numberLnaGainSteps = 0, 
		.lnaGainSteps_mdB = { 0, 0, 0, 0  }, 
		.lnaDigitalGainDelay = 0, 
		.minGainIndex = 0, 
		.lnaType = ADI_ADRV9001_EXTERNAL_LNA_TYPE_SINGLE }
				, 
		.rxSsiConfig = { 
		.ssiType = ADI_ADRV9001_SSI_TYPE_DISABLE, 
		.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_2_BIT_SYMBOL_DATA, 
		.numLaneSel = ADI_ADRV9001_SSI_1_LANE, 
		.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DISABLED, 
		.lvdsIBitInversion = false, 
		.lvdsQBitInversion = false, 
		.lvdsStrobeBitInversion = false, 
		.lvdsUseLsbIn12bitMode = 0, 
		.lvdsRxClkInversionEn = false, 
		.cmosDdrPosClkEn = false, 
		.cmosClkInversionEn = false, 
		.ddrEn = false, 
		.rxMaskStrobeEn = false }
				 }
			 }  } }
	, 
		.tx = { 
		.txInitChannelMask = 12, 
		.txProfile = { { 
		.primarySigBandwidth_Hz = 30000000, 
		.txInputRate_Hz = 60000000, 
		.txInterfaceSampleRate_Hz = 60000000, 
		.txOffsetLo_kHz = 0, 
		.validDataDelay = 0, 
		.txBbf3dBCorner_kHz = 50000, 
		.outputSignaling = ADI_ADRV9001_TX_IQ, 
		.txPdBiasCurrent = 1, 
		.txPdGainEnable = 0, 
		.txPrePdRealPole_kHz = 1000000, 
		.txPostPdRealPole_kHz = 530000, 
		.txBbfPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH, 
		.txExtLoopBackType = 0, 
		.txExtLoopBackForInitCal = 0, 
		.txPeakLoopBackPower = 0, 
		.frequencyDeviation_Hz = 0, 
		.txDpProfile = { 
		.txPreProc = { 
		.txPreProcSymbol0 = 0, 
		.txPreProcSymbol1 = 0, 
		.txPreProcSymbol2 = 0, 
		.txPreProcSymbol3 = 0, 
		.txPreProcSymMapDivFactor = 1, 
		.txPreProcMode = ADI_ADRV9001_TX_DP_PREPROC_IQ_DATA_WITH_PFIRS, 
		.txPreProcWbNbPfirIBankSel = ADI_ADRV9001_PFIR_BANK_A, 
		.txPreProcWbNbPfirQBankSel = ADI_ADRV9001_PFIR_BANK_B }
				, 
		.txWbIntTop = { 
		.txInterpBy2Blk30En = 0, 
		.txInterpBy2Blk28En = 0, 
		.txInterpBy2Blk26En = 0, 
		.txInterpBy2Blk24En = 0, 
		.txInterpBy2Blk22En = 0, 
		.txWbLpfBlk22p1En = 0 }
				, 
		.txNbIntTop = { 
		.txInterpBy2Blk20En = 0, 
		.txInterpBy2Blk18En = 0, 
		.txInterpBy2Blk16En = 0, 
		.txInterpBy2Blk14En = 0, 
		.txInterpBy2Blk12En = 0, 
		.txInterpBy3Blk10En = 0, 
		.txInterpBy2Blk8En = 0, 
		.txScicBlk32En = 0, 
		.txScicBlk32DivFactor = 1 }
				, 
		.txIntTop = { 
		.interpBy3Blk44p1En = 1, 
		.sinc3Blk44En = 0, 
		.sinc2Blk42En = 0, 
		.interpBy3Blk40En = 1, 
		.interpBy2Blk38En = 0, 
		.interpBy2Blk36En = 0 }
				, 
		.txIntTopFreqDevMap = { 
		.rrc2Frac = 0, 
		.mpll = 0, 
		.nchLsw = 0, 
		.nchMsb = 0, 
		.freqDevMapEn = 0, 
		.txRoundEn = 1 }
				, 
		.txIqdmDuc = { 
		.iqdmDucMode = ADI_ADRV9001_TX_DP_IQDMDUC_MODE2, 
		.iqdmDev = 0, 
		.iqdmDevOffset = 0, 
		.iqdmScalar = 0, 
		.iqdmThreshold = 0, 
		.iqdmNco = { 
		.freq = 0, 
		.sampleFreq = 60000000, 
		.phase = 0, 
		.realOut = 0 }
					 }
				 }
			, 
		.txSsiConfig = { 
		.ssiType = ADI_ADRV9001_SSI_TYPE_LVDS, 
		.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA, 
		.numLaneSel = ADI_ADRV9001_SSI_2_LANE, 
		.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DCLK_OUT_ENABLED, 
		.lvdsIBitInversion = false, 
		.lvdsQBitInversion = false, 
		.lvdsStrobeBitInversion = false, 
		.lvdsUseLsbIn12bitMode = 0, 
		.lvdsRxClkInversionEn = false, 
		.cmosDdrPosClkEn = false, 
		.cmosClkInversionEn = false, 
		.ddrEn = true, 
		.rxMaskStrobeEn = false }
			 }, { 
		.primarySigBandwidth_Hz = 30000000, 
		.txInputRate_Hz = 60000000, 
		.txInterfaceSampleRate_Hz = 60000000, 
		.txOffsetLo_kHz = 0, 
		.validDataDelay = 0, 
		.txBbf3dBCorner_kHz = 50000, 
		.outputSignaling = ADI_ADRV9001_TX_IQ, 
		.txPdBiasCurrent = 1, 
		.txPdGainEnable = 0, 
		.txPrePdRealPole_kHz = 1000000, 
		.txPostPdRealPole_kHz = 530000, 
		.txBbfPower = ADI_ADRV9001_COMPONENT_POWER_LEVEL_HIGH, 
		.txExtLoopBackType = 0, 
		.txExtLoopBackForInitCal = 0, 
		.txPeakLoopBackPower = 0, 
		.frequencyDeviation_Hz = 0, 
		.txDpProfile = { 
		.txPreProc = { 
		.txPreProcSymbol0 = 0, 
		.txPreProcSymbol1 = 0, 
		.txPreProcSymbol2 = 0, 
		.txPreProcSymbol3 = 0, 
		.txPreProcSymMapDivFactor = 1, 
		.txPreProcMode = ADI_ADRV9001_TX_DP_PREPROC_IQ_DATA_WITH_PFIRS, 
		.txPreProcWbNbPfirIBankSel = ADI_ADRV9001_PFIR_BANK_C, 
		.txPreProcWbNbPfirQBankSel = ADI_ADRV9001_PFIR_BANK_D }
				, 
		.txWbIntTop = { 
		.txInterpBy2Blk30En = 0, 
		.txInterpBy2Blk28En = 0, 
		.txInterpBy2Blk26En = 0, 
		.txInterpBy2Blk24En = 0, 
		.txInterpBy2Blk22En = 0, 
		.txWbLpfBlk22p1En = 0 }
				, 
		.txNbIntTop = { 
		.txInterpBy2Blk20En = 0, 
		.txInterpBy2Blk18En = 0, 
		.txInterpBy2Blk16En = 0, 
		.txInterpBy2Blk14En = 0, 
		.txInterpBy2Blk12En = 0, 
		.txInterpBy3Blk10En = 0, 
		.txInterpBy2Blk8En = 0, 
		.txScicBlk32En = 0, 
		.txScicBlk32DivFactor = 1 }
				, 
		.txIntTop = { 
		.interpBy3Blk44p1En = 1, 
		.sinc3Blk44En = 0, 
		.sinc2Blk42En = 0, 
		.interpBy3Blk40En = 1, 
		.interpBy2Blk38En = 0, 
		.interpBy2Blk36En = 0 }
				, 
		.txIntTopFreqDevMap = { 
		.rrc2Frac = 0, 
		.mpll = 0, 
		.nchLsw = 0, 
		.nchMsb = 0, 
		.freqDevMapEn = 0, 
		.txRoundEn = 1 }
				, 
		.txIqdmDuc = { 
		.iqdmDucMode = ADI_ADRV9001_TX_DP_IQDMDUC_MODE2, 
		.iqdmDev = 0, 
		.iqdmDevOffset = 0, 
		.iqdmScalar = 0, 
		.iqdmThreshold = 0, 
		.iqdmNco = { 
		.freq = 0, 
		.sampleFreq = 60000000, 
		.phase = 0, 
		.realOut = 0 }
					 }
				 }
			, 
		.txSsiConfig = { 
		.ssiType = ADI_ADRV9001_SSI_TYPE_LVDS, 
		.ssiDataFormatSel = ADI_ADRV9001_SSI_FORMAT_16_BIT_I_Q_DATA, 
		.numLaneSel = ADI_ADRV9001_SSI_2_LANE, 
		.strobeType = ADI_ADRV9001_SSI_SHORT_STROBE, 
		.lsbFirst = 0, 
		.qFirst = 0, 
		.txRefClockPin = ADI_ADRV9001_SSI_TX_REF_CLOCK_PIN_DCLK_OUT_ENABLED, 
		.lvdsIBitInversion = false, 
		.lvdsQBitInversion = false, 
		.lvdsStrobeBitInversion = false, 
		.lvdsUseLsbIn12bitMode = 0, 
		.lvdsRxClkInversionEn = false, 
		.cmosDdrPosClkEn = false, 
		.cmosClkInversionEn = false, 
		.ddrEn = true, 
		.rxMaskStrobeEn = false }
			 }  } }
	, 
		.sysConfig = { 
		.duplexMode = ADI_ADRV9001_TDD_MODE, 
		.fhModeOn = 0, 
		.numDynamicProfiles = ADI_ADRV9001_NUM_DYNAMIC_PROFILES_DISABLED, 
		.mcsMode = ADI_ADRV9001_MCSMODE_DISABLED, 
		.mcsInterfaceType = ADI_ADRV9001_SSI_TYPE_DISABLE, 
		.adcTypeMonitor = ADI_ADRV9001_ADC_HP, 
		.pllLockTime_us = 15, 
		.pllPhaseSyncWait_us = 0, 
		.pllModulus = { 
		.modulus = { 8388593, 8388593, 8388593, 8388593, 8388593  }, 
		.dmModulus = { 8388593, 8388593  } }
		, 
		.warmBootEnable = false }
	, 
		.pfirBuffer = { 
		.pfirRxWbNbChFilterCoeff_A = { 
		.numCoeff = 128, 
		.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC, 
		.tapsSel = ADI_ADRV9001_PFIR_128_TAPS | ADI_ADRV9001_PFIR_TAPS_MAX_ID, 
		.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  } }
		, 
		.pfirRxWbNbChFilterCoeff_B = { 
		.numCoeff = 128, 
		.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC, 
		.tapsSel = ADI_ADRV9001_PFIR_128_TAPS | ADI_ADRV9001_PFIR_TAPS_MAX_ID, 
		.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  } }
		, 
		.pfirRxWbNbChFilterCoeff_C = { 
		.numCoeff = 128, 
		.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC, 
		.tapsSel = ADI_ADRV9001_PFIR_128_TAPS | ADI_ADRV9001_PFIR_TAPS_MAX_ID, 
		.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  } }
		, 
		.pfirRxWbNbChFilterCoeff_D = { 
		.numCoeff = 128, 
		.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC, 
		.tapsSel = ADI_ADRV9001_PFIR_128_TAPS | ADI_ADRV9001_PFIR_TAPS_MAX_ID, 
		.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  } }
		, 
		.pfirTxWbNbPulShpCoeff_A = { 
		.numCoeff = 128, 
		.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC, 
		.tapsSel = ADI_ADRV9001_PFIR_128_TAPS | ADI_ADRV9001_PFIR_TAPS_MAX_ID, 
		.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  } }
		, 
		.pfirTxWbNbPulShpCoeff_B = { 
		.numCoeff = 128, 
		.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC, 
		.tapsSel = ADI_ADRV9001_PFIR_128_TAPS | ADI_ADRV9001_PFIR_TAPS_MAX_ID, 
		.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  } }
		, 
		.pfirTxWbNbPulShpCoeff_C = { 
		.numCoeff = 128, 
		.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC, 
		.tapsSel = ADI_ADRV9001_PFIR_128_TAPS | ADI_ADRV9001_PFIR_TAPS_MAX_ID, 
		.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  } }
		, 
		.pfirTxWbNbPulShpCoeff_D = { 
		.numCoeff = 128, 
		.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC, 
		.tapsSel = ADI_ADRV9001_PFIR_128_TAPS | ADI_ADRV9001_PFIR_TAPS_MAX_ID, 
		.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  } }
		, 
		.pfirRxNbPulShp = { { 
		.numCoeff = 128, 
		.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC, 
		.taps = 128, 
		.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  } }, { 
		.numCoeff = 128, 
		.symmetricSel = ADI_ADRV9001_PFIR_COEF_NON_SYMMETRIC, 
		.taps = 128, 
		.gainSel = ADI_ADRV9001_PFIR_GAIN_ZERO_DB, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 8388608, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  } }  }, 
		.pfirRxMagLowTiaLowSRHp = { { 
		.numCoeff = 21, 
		.coefficients = { -24, 121, -377, 870, -1662, 2759, -4058, 5367, -6335, 5623, 28195, 5623, -6335, 5367, -4058, 2759, -1662, 870, -377, 121, -24  } }, { 
		.numCoeff = 21, 
		.coefficients = { -24, 121, -377, 870, -1662, 2759, -4058, 5367, -6335, 5623, 28195, 5623, -6335, 5367, -4058, 2759, -1662, 870, -377, 121, -24  } }  }, 
		.pfirRxMagLowTiaHighSRHp = { { 
		.numCoeff = 21, 
		.coefficients = { -54, 147, 162, -829, 84, 1861, -20, -4552, -1428, 11402, 19222, 11402, -1428, -4552, -20, 1861, 84, -829, 162, 147, -54  } }, { 
		.numCoeff = 21, 
		.coefficients = { -54, 147, 162, -829, 84, 1861, -20, -4552, -1428, 11402, 19222, 11402, -1428, -4552, -20, 1861, 84, -829, 162, 147, -54  } }  }, 
		.pfirRxMagHighTiaHighSRHp = { { 
		.numCoeff = 21, 
		.coefficients = { 35, -190, 547, -1030, 1238, -501, -1767, 5470, -9255, 8699, 26276, 8699, -9255, 5470, -1767, -501, 1238, -1030, 547, -190, 35  } }, { 
		.numCoeff = 21, 
		.coefficients = { 35, -190, 547, -1030, 1238, -501, -1767, 5470, -9255, 8699, 26276, 8699, -9255, 5470, -1767, -501, 1238, -1030, 547, -190, 35  } }  }, 
		.pfirRxMagLowTiaLowSRLp = { { 
		.numCoeff = 21, 
		.coefficients = { -24, 121, -377, 870, -1662, 2758, -4056, 5366, -6333, 5625, 28191, 5625, -6333, 5366, -4056, 2758, -1662, 870, -377, 121, -24  } }, { 
		.numCoeff = 21, 
		.coefficients = { -24, 121, -377, 870, -1662, 2758, -4056, 5366, -6333, 5625, 28191, 5625, -6333, 5366, -4056, 2758, -1662, 870, -377, 121, -24  } }  }, 
		.pfirRxMagLowTiaHighSRLp = { { 
		.numCoeff = 21, 
		.coefficients = { -54, 147, 162, -828, 84, 1859, -22, -4547, -1423, 11399, 19212, 11399, -1423, -4547, -22, 1859, 84, -828, 162, 147, -54  } }, { 
		.numCoeff = 21, 
		.coefficients = { -54, 147, 162, -828, 84, 1859, -22, -4547, -1423, 11399, 19212, 11399, -1423, -4547, -22, 1859, 84, -828, 162, 147, -54  } }  }, 
		.pfirRxMagHighTiaHighSRLp = { { 
		.numCoeff = 21, 
		.coefficients = { 35, -190, 546, -1027, 1235, -501, -1758, 5450, -9230, 8701, 26249, 8701, -9230, 5450, -1758, -501, 1235, -1027, 546, -190, 35  } }, { 
		.numCoeff = 21, 
		.coefficients = { 35, -190, 546, -1027, 1235, -501, -1758, 5450, -9230, 8701, 26249, 8701, -9230, 5450, -1758, -501, 1235, -1027, 546, -190, 35  } }  }, 
		.pfirTxMagComp1 = { 
		.numCoeff = 21, 
		.coefficients = { 64, -326, 870, -1405, 1008, 1605, -6922, 13144, -13030, 3744, 35269, 3744, -13030, 13144, -6922, 1605, 1008, -1405, 870, -326, 64  } }
		, 
		.pfirTxMagComp2 = { 
		.numCoeff = 21, 
		.coefficients = { 64, -326, 870, -1405, 1008, 1605, -6922, 13144, -13030, 3744, 35269, 3744, -13030, 13144, -6922, 1605, 1008, -1405, 870, -326, 64  } }
		, 
		.pfirTxMagCompNb = { { 
		.numCoeff = 13, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  } }, { 
		.numCoeff = 13, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  } }  }, 
		.pfirRxMagCompNb = { { 
		.numCoeff = 13, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  } }, { 
		.numCoeff = 13, 
		.coefficients = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0  } }  } }
	 };
