import FWCore.ParameterSet.Config as cms


process = cms.Process("Demo")
process.load('Configuration.Geometry.GeometryExtended2026D49Reco_cff')
process.load('Configuration.StandardSequences.MagneticField_0T_cff')
process.load('Configuration.StandardSequences.Services_cff')
process.load('Configuration.EventContent.EventContent_cff')
process.load("FWCore.MessageService.MessageLogger_cfi")
process.load('Configuration.StandardSequences.FrontierConditions_GlobalTag_cff')
process.load('Configuration.StandardSequences.EndOfProcess_cff')
process.load('HGCal_analysis.HGCal_analyzer.HGCal_analysis_cfi')
#process.load("RecoLocalCalo.HGCalRecProducers.hgcalLayerClusters_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )


import FWCore.Utilities.FileUtils as FileUtils
readFiles = cms.untracked.vstring()
#readFiles.extend(FileUtils.loadListFromFile ('INPUTFILELIST') )

process.source = cms.Source("PoolSource",
                            # replace 'myfile.root' with the source file you want to use
                            fileNames = cms.untracked.vstring('file:/afs/desy.de/user/g/gmilella/HGCal_sim/CMSSW_12_1_0_pre4/src/SingleElectron_RECO.root'),#readfiles,
                            duplicateCheckMode = cms.untracked.string("noDuplicateCheck"),
                            inputCommands=cms.untracked.vstring('keep *',
                                                                'drop EcalEBTriggerPrimitiveDigisSorted_simEcalEBTriggerPrimitiveDigis_*_HLT'
                                                            )
                        )


from HGCal_analysis.HGCal_analyzer.HGCal_analysis_cfi import HGCalParameters

process.ana = cms.EDAnalyzer('HGCal_analyzer',
                             detector = cms.string("all"),
                             rawRecHits = cms.bool(True),                              
                             #CFDTimeCorrection = cms.int32(CFDVal),
                             #cellType = cms.int32(CELLT),
                             #floorValue = cms.double(FLOORV),
                             #lifeAge = cms.int32(LIFEA),
                             #absTrend = cms.double(ABSTREND),
                             #HGCEEInput = cms.InputTag('HGCalRecHit:HGCEERecHits'),
                             #HGCFHInput = cms.InputTag('HGCalRecHit:HGCHEFRecHits'),
                             HGCBHInput = cms.InputTag('HGCalRecHit:HGCHEBRecHits'),
                             dEdXweights = HGCalParameters.dEdXweights,
                             thicknessCorrection = HGCalParameters.thicknessCorrection,
                             #HGCEE_fCPerMIP = HGCalTimeEstimator.HGCEE_fCPerMIP,
                             #HGCEE_noisefC = HGCalTimeEstimator.HGCEE_noisefC,
                             #HGCEF_noisefC = HGCalTimeEstimator.HGCEF_noisefC,
                             HGCBH_noiseMIP = HGCalParameters.HGCBH_noiseMIP,
                             nonAgedNoises = HGCalParameters.nonAgedNoises,
                             endOfLifeNoises = HGCalParameters.endOfLifeNoises,
                             #HGCEE_keV2fC  = HGCalTimeEstimator.HGCEE_keV2fC,
                             #HGCHEF_keV2fC = HGCalTimeEstimator.HGCHEF_keV2fC,
                             HGCHB_keV2MIP = HGCalParameters.HGCHB_keV2MIP
                         )

process.TFileService = cms.Service("TFileService",
                                   fileName = cms.string("file:OUTFILE.root")
)
process.p = cms.Path(process.ana)
