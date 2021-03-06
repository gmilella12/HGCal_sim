import FWCore.ParameterSet.Config as cms


from RecoLocalCalo.HGCalRecProducers.HGCalRecHit_cfi import dEdX, HGCalRecHit
from RecoLocalCalo.HGCalRecProducers.HGCalUncalibRecHit_cfi import HGCalUncalibRecHit
from SimCalorimetry.HGCalSimProducers.hgcalDigitizer_cfi import * #hgchebackDigitizer, nonAgedNoises, endOfLifeNoises #hgceeDigitizer, hgchefrontDigitizer

HGCalParameters = cms.PSet(
    
    dEdXweights = cms.vdouble(dEdX.weights),
    thicknessCorrection = cms.vdouble(HGCalRecHit.thicknessCorrection),
        
    HGCBH_noiseMIP = hgchebackDigitizer.digiCfg.noise, ##BH

    nonAgedNoises = cms.vdouble(nonAgedNoises),
    endOfLifeNoises = cms.vdouble(endOfLifeNoises),

    HGCHB_keV2MIP = hgchebackDigitizer.digiCfg.keV2MIP ##BH

)


