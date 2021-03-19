import ROOT

newIdx = {}

# transparent grey for uncertainty bands (as in HIG-14-034)
colorToAdapt = ROOT.gROOT.GetColor(16)
transGrey_idx = ROOT.gROOT.GetListOfColors().GetLast() + 1
transGrey = ROOT.TColor(transGrey_idx, colorToAdapt.GetRed(), colorToAdapt.GetGreen(), colorToAdapt.GetBlue(), "", 0.02)

newIdx["transgrey"] = transGrey_idx
