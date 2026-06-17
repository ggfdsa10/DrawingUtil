/////////////////////////////////////////////////////
//          ROOT based figure drawing uility       //
//  Author: Seunghwan Lee (zxzfdsa@gmail.com)      //
/////////////////////////////////////////////////////

#include "DrawingUtil.hh"

DrawingUtil::DrawingUtil(TString figurePath)
{
    mFigurePath = figurePath;
    if(mFigurePath[mFigurePath.Sizeof()-2] != '/'){mFigurePath = mFigurePath + "/";}
    mCObjIdx = 0;
    mObjIdx = 0;

    mColorPaletteIdx = 0;
    mMarkerStyleIdx = 0;
}

DrawingUtil::~DrawingUtil()
{
}

void DrawingUtil::Init()
{

}

void DrawingUtil::Clear(TString opt)
{
    if(mCanvas != nullptr){delete mCanvas;}
    mDrawOptArr.clear();
    mLabelName.clear();
    mHist.clear();
    mHist2D.clear();
    mGraph.clear();

    bool isClearLegend = false;
    bool isClearText = false;
    opt.ToUpper();
    if(opt.Index("TEXT") != -1 || opt.Index("LATEX") != -1 || opt.Index("TXT") != -1 || opt.Index("ALL") != -1){isClearText = true;}
    if(opt.Index("LEG") != -1 || opt.Index("ALL") != -1){isClearLegend = true;}

    vector<InfoText> tmpLatex;
    vector<InfoText> tmpLegend;
    tmpLatex = mLatex;
    tmpLegend = mLegend;

    mLatex.clear();
    mLegend.clear();

    if(!isClearText){
        int latexNum = tmpLatex.size();
        for(int i=0; i<latexNum; i++){
            if(tmpLatex[i].persistency){
                mLatex.push_back(tmpLatex[i]);
            }
        }
    }
    if(!isClearLegend){
        int legendNum = tmpLegend.size();
        for(int i=0; i<legendNum; i++){
            if(tmpLegend[i].persistency){
                mLegend.push_back(tmpLegend[i]);
            }
        }
    }
}

void DrawingUtil::DrawHist(TString opt)
{
    InitDrawOption(opt);

    int cNum = 0;
    InitHist(cNum);

    int histNum = mHist.size();
    for(int c=0; c<cNum; c++){
        mCanvas -> cd(c+1);

        if(GetDrawFlag("xLog") || GetDrawFlag("xLog", c)){gPad -> SetLogx();}
        if(GetDrawFlag("yLog") || GetDrawFlag("yLog", c)){gPad -> SetLogy();}

        gPad -> SetTopMargin(0.03);
        gPad -> SetLeftMargin(0.135);
        gPad -> SetBottomMargin(0.11);

        double minBin;
        double maxBin;
        for(int i=0; i<histNum; i++){
            if(mHist[i].base.cIdx == c && mHist[i].base.objIdx == -1){
                mHist[i].hist -> GetYaxis()->SetTitleSize(0.045);
                mHist[i].hist -> GetXaxis()->SetTitleSize(0.045);
                mHist[i].hist -> GetYaxis()->SetTitleOffset(1.6);
                mHist[i].hist -> GetXaxis()->SetTitleOffset(1.);
                mHist[i].hist -> Draw();

                int nBinsX = mHist[i].hist ->GetNbinsX();
                minBin = mHist[i].hist -> GetBinLowEdge(1);
                maxBin = mHist[i].hist -> GetBinLowEdge(nBinsX) + mHist[i].hist->GetBinWidth(nBinsX);
            }
        }
        for(int i=0; i<histNum; i++){
            if(mHist[i].base.cIdx == c && mHist[i].base.objIdx != -1){
                mHist[i].hist -> Draw("same");
            }
        }
        DrawText(c);
        DrawLegend(c);

        if(GetDrawFlag("ratio") || GetDrawFlag("ratio", c)){
            if(GetDrawFlag("xaxis1") || GetDrawFlag("xaxis2")){
                TString xaxis1 = GetDrawValue("XAXIS1");
                TString xaxis2 = GetDrawValue("XAXIS2");
                minBin = (GetDrawFlag("xaxis1"))? xaxis1.Atof() : minBin;
                maxBin = (GetDrawFlag("xaxis2"))? xaxis2.Atof() : maxBin;
            }
            if(GetDrawFlag("xaxis1", c) || GetDrawFlag("xaxis2", c)){
                TString xaxis1 = GetDrawValue("XAXIS1", c);
                TString xaxis2 = GetDrawValue("XAXIS2", c);
                minBin = (GetDrawFlag("xaxis1", c))? xaxis1.Atof() : minBin;
                maxBin = (GetDrawFlag("xaxis2", c))? xaxis2.Atof() : maxBin;
            }

            TLine* ratioLine = new TLine(minBin, 1., maxBin, 1.);
            ratioLine -> SetLineColor(kBlack);
            ratioLine -> SetLineStyle(2);
            ratioLine -> SetLineWidth(1);
            ratioLine -> Draw("same");
        }
    }
}

void DrawingUtil::DrawHistWithRatio(int baseIdx, TString opt, TString yTitle)
{
    InitDrawOption(opt);
    int cNum = 0;
    InitHist(cNum);

    int histNum = mHist.size();
    for(int c=0; c<cNum; c++){
        mCanvas -> cd(c+1);

        TH1D* bkgRatioHist;
        TH1D* baseRatioHist;
        vector<TH1D*> ratioHistArr;

        TString canvasName = mCanvas -> GetName();
        TPad* mainPad = new TPad(Form("%s_mainPad_c%i", canvasName.Data(), c), "", 0.1, 0.3, 0.9, 0.9);  
        mainPad -> Draw();
        mainPad -> SetTopMargin(0.03);
        mainPad -> SetLeftMargin(0.135);
        mainPad -> SetBottomMargin(0.2);
        mainPad -> SetGrid(0,0);

        TPad* ratioPad = new TPad(Form("%s_ratioPad_c%i", canvasName.Data(), c), "", 0.1, 0.1, 0.9, 0.415+0.005);  
        ratioPad -> Draw();
        ratioPad -> SetTopMargin(0);
        ratioPad -> SetLeftMargin(0.135);
        ratioPad -> SetBottomMargin(0.3);
        ratioPad -> SetGrid(0,0);

        mainPad -> cd();
        if(GetDrawFlag("xLog") || GetDrawFlag("xLog", c)){mainPad -> SetLogx();}
        if(GetDrawFlag("yLog") || GetDrawFlag("yLog", c)){gPad -> SetLogy();}

        for(int i=0; i<histNum; i++){
            if(mHist[i].base.cIdx == c && mHist[i].base.objIdx == -1){
                mHist[i].hist -> GetYaxis()->SetTitleSize(0.045);
                mHist[i].hist -> GetXaxis()->SetTitleSize(0);
                mHist[i].hist -> GetYaxis()->SetTitleOffset(1.6);
                mHist[i].hist -> GetXaxis()->SetTitleOffset(0.);
                mHist[i].hist -> Draw();

                bkgRatioHist = (TH1D*)mHist[i].hist -> Clone(Form("%s_bkgRatioHist", mHist[i].hist->GetName()));
            }
        }
        for(int i=0; i<histNum; i++){
            if(mHist[i].base.cIdx == c && mHist[i].base.objIdx != -1){
                mHist[i].hist -> Draw("same");

                if(mHist[i].base.objIdx == baseIdx){
                    mHist[i].hist -> SetLineWidth(2);
                    baseRatioHist = (TH1D*)mHist[i].hist -> Clone(Form("%s_RatioBaseHist_c%i", mHist[i].hist, c));
                }
                else{
                    mHist[i].hist -> SetLineWidth(1);
                    TH1D* ratioHist = (TH1D*)mHist[i].hist -> Clone(Form("%s_RatioHist_c%i_%i", mHist[i].hist, c, i));
                    ratioHistArr.push_back(ratioHist);
                }
            }
        }

        DrawText(c);
        DrawLegend(c);

        ratioPad -> cd();
        bkgRatioHist -> GetYaxis()->SetTitleOffset(0.95);
        bkgRatioHist -> GetXaxis()->SetTitleOffset(1.2);
        bkgRatioHist -> GetYaxis() -> SetTitleSize(0.08);
        bkgRatioHist -> GetXaxis() -> SetTitleSize(0.08);
        bkgRatioHist -> GetYaxis() -> SetLabelSize(0.06);
        bkgRatioHist -> GetXaxis() -> SetLabelSize(0.07);
        bkgRatioHist -> GetXaxis() -> SetLabelOffset(0.02);
        bkgRatioHist -> GetYaxis()->SetRangeUser(0.001, 1.999);
               
        TString xTitleTmp = bkgRatioHist -> GetXaxis()->GetTitle();
        bkgRatioHist -> SetTitle(Form(";%s;%s", xTitleTmp.Data(), yTitle.Data()));
        bkgRatioHist -> Draw();

        int ratioHistNum = ratioHistArr.size();
        int binNum = baseRatioHist -> GetNbinsX();

        for(int i=0; i<ratioHistNum; i++){
            for(int b=0; b<binNum; b++){
                double entry = ratioHistArr[i] -> GetBinContent(b+1);
                double baseEntry = baseRatioHist -> GetBinContent(b+1);

                double ratio = entry / baseEntry;
                if(baseEntry < 1.e-4 || entry < 1.e-4){ratio = 0.;}
                if(ratio >= 1.999){ratio = 2.;}
                ratioHistArr[i] -> SetBinContent(b+1, ratio);
            }
            ratioHistArr[i] -> SetMarkerColor(ratioHistArr[i]->GetLineColor());
            ratioHistArr[i] -> Draw("same");
        }

        double nBinsX = bkgRatioHist -> GetNbinsX();
        double binLower = bkgRatioHist -> GetBinLowEdge(1);
        double binHigher = bkgRatioHist -> GetBinLowEdge(nBinsX) + bkgRatioHist->GetBinWidth(nBinsX);

        if(GetDrawFlag("xaxis1") || GetDrawFlag("xaxis2")){
            TString xaxis1 = GetDrawValue("XAXIS1");
            TString xaxis2 = GetDrawValue("XAXIS2");
            binLower = (GetDrawFlag("xaxis1"))? xaxis1.Atof() : binLower;
            binHigher = (GetDrawFlag("xaxis2"))? xaxis2.Atof() : binHigher;
        }
        if(GetDrawFlag("xaxis1", c) || GetDrawFlag("xaxis2", c)){
            TString xaxis1 = GetDrawValue("XAXIS1", c);
            TString xaxis2 = GetDrawValue("XAXIS2", c);
            binLower = (GetDrawFlag("xaxis1", c))? xaxis1.Atof() : binLower;
            binHigher = (GetDrawFlag("xaxis2", c))? xaxis2.Atof() : binHigher;
        }

        TLine* ratioLine = new TLine(binLower, 1., binHigher, 1.);
        ratioLine -> SetLineColor(kBlack);
        ratioLine -> SetLineStyle(2);
        ratioLine -> SetLineWidth(1);
        ratioLine -> Draw("same");
    }
}

void DrawingUtil::DrawHist2D(TString opt)
{
    InitDrawOption(opt);
    int cNum = 0;
    InitHist2D(cNum);

    SetCanvas(cNum);

    int histNum = mHist2D.size();
    for(int c=0; c<cNum; c++){
        mCanvas -> cd(c+1);

        if(GetDrawFlag("xLog") || GetDrawFlag("xLog", c)){gPad -> SetLogx();}
        if(GetDrawFlag("yLog") || GetDrawFlag("yLog", c)){gPad -> SetLogy();}
        if(GetDrawFlag("zLog") || GetDrawFlag("zLog", c)){gPad -> SetLogz();}

        gPad -> SetTopMargin(0.03);
        gPad -> SetLeftMargin(0.135);
        gPad -> SetBottomMargin(0.11);

        mHist2D[c].hist -> GetYaxis()->SetTitleSize(0.045);
        mHist2D[c].hist -> GetXaxis()->SetTitleSize(0.045);
        mHist2D[c].hist -> GetYaxis()->SetTitleOffset(1.6);
        mHist2D[c].hist -> GetXaxis()->SetTitleOffset(1.);
        mHist2D[c].hist -> Draw("colz");

        DrawText(c);
    }
}

void DrawingUtil::DrawGraph(TString opt)
{
    InitDrawOption(opt);
    int cNum = 0;
    InitGraph(cNum);

    int graphNum = mGraph.size();
    for(int c=0; c<cNum; c++){
        mCanvas -> cd(c+1);
        gPad -> SetTopMargin(0.03);
        gPad -> SetLeftMargin(0.135);
        gPad -> SetBottomMargin(0.11);

        double minX, maxX, minY, maxY;
        for(int i=0; i<graphNum; i++){
            if(mGraph[i].base.cIdx == c && mGraph[i].base.objIdx == -1){
                mGraph[i].graph -> GetYaxis()->SetTitleSize(0.045);
                mGraph[i].graph -> GetXaxis()->SetTitleSize(0.045);
                mGraph[i].graph -> GetYaxis()->SetTitleOffset(1.6);
                mGraph[i].graph -> GetXaxis()->SetTitleOffset(1.);
                mGraph[i].graph -> Draw("ap");

                mGraph[i].graph->GetPoint(0, minX, minY);
                mGraph[i].graph->GetPoint(mGraph[i].graph->GetN()-1, maxX, maxY);
            }
            for(int i=0; i<graphNum; i++){
                if(mGraph[i].base.cIdx == c && mGraph[i].base.objIdx != -1){
                    mGraph[i].graph -> Draw("same, p");
                }
            }
        }
        DrawText(c);
        DrawLegend(c);

        if(GetDrawFlag("Ratio") || GetDrawFlag("Ratio", c)){
            double diffX = (maxX-minX)/10.;
            TLine* ratioLine = new TLine(minX-diffX, 1., maxX+diffX, 1.);
            ratioLine -> SetLineColor(kBlack);
            ratioLine -> SetLineStyle(2);
            ratioLine -> SetLineWidth(1);
            ratioLine -> Draw("same");

        }
    }
}


void DrawingUtil::SaveFigure(TString name)
{
    TString figureFormat = ".pdf";

    TString saveName = mFigurePath;
    if(name.Index("/") != -1){saveName = name;}
    else{saveName = saveName + name;}

    TObjArray *tokens = saveName.Tokenize(".");
    int entries = tokens -> GetEntries();
    if(entries == 0){
        saveName = saveName + figureFormat;
    }
    else{
        TString tmpFormat = ((TObjString *)tokens->At(entries-1))->GetString();
        tmpFormat.ToUpper();
        if(tmpFormat.Index("PDF") == -1 && tmpFormat.Index("PNG") == -1 && tmpFormat.Index("PDF") == -1){
            saveName = saveName + figureFormat;
        }
    }

    mCanvas -> Draw();
    mCanvas -> SaveAs(Form("%s", saveName.Data()));
}

void DrawingUtil::SetCanvas(int divideNum, int xPadNum, TString name, double size)
{   
    int xNum = 1;
    int yNum = 1;
    if(divideNum == 4){xNum = 2;}
    else if(divideNum < 4){xNum = divideNum;}
    else if(divideNum < 10){xNum = 3;}
    else if(divideNum == 8){xNum = 4;}
    else if(divideNum > 10){xNum = 4;}
    if(xPadNum != -1){xNum = xPadNum;}

    double tmp = double(divideNum)/double(xNum);
    yNum = int(ceil(tmp));
    if(name == ""){name = Form("drawinUtil_canvas_%i_%i_%i_%i",xNum, yNum, divideNum, mCObjIdx);}
    mCanvas = new TCanvas(name, "", double(xNum)*size, double(yNum)*size);
    mCanvas -> Divide(xNum, yNum);
    mCObjIdx++;
}

void DrawingUtil::SetXLabelName(int i, TString name)
{
    mLabelName.push_back(make_pair(i, name));
}

void DrawingUtil::SetTH1DRatio(int num, double* y, double base, TString title, int cIdx, int colorIdx, int markerIdx)
{
    InitDrawOption("ratio");
    TString name = Form("th1dRatio_%s_c%i_%i", title.Data(), cIdx, mObjIdx++);
    TH1D* hist = new TH1D(name, "", num, 0., double(num));
    hist -> Sumw2();

    for(int b=0; b<num; b++){
        double ratio = y[b]/base;
        double variance = (y[b]+1.)*(y[b]+2.)/((base+2.)*(base+3.)) - (y[b]+1.)*(y[b]+1.)/((base+2.)*(base+2.)); // following Bayesian statistics for efficiency
        double sigma = sqrt(variance);
    }

    SetTH1D(hist, title, cIdx, colorIdx, markerIdx);
}

void DrawingUtil::SetTH1D(TH1D* hist, TString title, int cIdx, int colorIdx, int markerIdx)
{
    int histNum = mHist.size();
    int objNum = 0;
    for(int i=0; i<histNum; i++){
        if(mHist[i].base.cIdx == cIdx){
            objNum++;
        }
    }

    InfoHist histInfo;

    TH1D* clone = (TH1D*)hist -> Clone(Form("%s_clone_th1d_c%i_obj%i_%i", TString(hist->GetName()).Data(), cIdx, objNum, mObjIdx++));
    histInfo.hist = clone;

    int color = (colorIdx == -1)? GetColor(objNum) : colorIdx;
    hist -> SetLineColor(color);
    hist -> SetMarkerColor(color);
    hist -> SetLineWidth(2.5);
    histInfo.hist -> SetLineColor(color);
    histInfo.hist -> SetMarkerColor(color);

    if(markerIdx != -1){
        hist -> SetMarkerStyle(GetMarker(markerIdx));
        histInfo.hist -> SetMarkerStyle(GetMarker(markerIdx));
    }
    histInfo.base.Clear();
    histInfo.base.title = title;
    histInfo.base.cIdx = cIdx;
    histInfo.base.objIdx = objNum;
    histInfo.base.colorIdx = colorIdx;
    histInfo.base.markerIdx = markerIdx;

    mHist.push_back(histInfo);
}

TH1D* DrawingUtil::GetTH1D(int cIdx, int objIdx)
{
    int histNum = mHist.size();
     for(int i=0; i<histNum; i++){
        if(mHist[i].base.cIdx == cIdx){
            if(mHist[i].base.objIdx == objIdx){
                return mHist[i].hist;
            }
        }
    }
    return 0;
}

void DrawingUtil::SetTH2D(TH2D* hist, TString title)
{
    int histNum = mHist2D.size();
    InfoHist2D histInfo;

    TH2D* clone = (TH2D*)hist -> Clone(Form("%s_clone_th2d_%i_%i", TString(hist->GetName()).Data(), histNum, mObjIdx++));
    histInfo.hist = clone;

    histInfo.base.Clear();
    histInfo.base.title = title;
    histInfo.base.cIdx = histNum;
    mHist2D.push_back(histInfo);
}

TH2D* DrawingUtil::GetTH2D(int cIdx)
{
    int histNum = mHist2D.size();
     for(int i=0; i<histNum; i++){
        if(i == cIdx){
            return mHist2D[i].hist;
        }
    }
    return 0; 
}

void DrawingUtil::SetGraph(int num, double* x, double* y, TString title, int cIdx, int colorIdx, int markerIdx)
{
    TGraphAsymmErrors* graph = SetGraph(title, cIdx, colorIdx, markerIdx);
    for(int i=0; i<num; i++){
        graph -> SetPoint(graph->GetN(), x[i], y[i]);
    }
}

void DrawingUtil::SetGraphRatio(int num, double* x, double* y, double* yBase, TString title, int cIdx, int colorIdx, int markerIdx)
{
    InitDrawOption("ratio");
    TGraphAsymmErrors* graph = SetGraph(title, cIdx, colorIdx, markerIdx);
    for(int i=0; i<num; i++){
        double ratio = y[i]/yBase[i];
        double variance = (y[i]+1.)*(y[i]+2.)/((yBase[i]+2.)*(yBase[i]+3.)) - (y[i]+1.)*(y[i]+1.)/((yBase[i]+2.)*(yBase[i]+2.)); // following Bayesian statistics for efficiency
        double sigma = sqrt(variance);

        if(yBase[i] < 3){continue;}
        graph -> SetPoint(graph->GetN(), x[i], ratio);
        graph -> SetPointError(graph->GetN()-1, 0., 0., sigma, sigma);
    }
}

void DrawingUtil::SetGraphAsymRatio(int num, double* x, double* y, double* yBase, TString title, int cIdx, int colorIdx, int markerIdx)
{
    InitDrawOption("ratio");
    TGraphAsymmErrors* graph = SetGraph(title, cIdx, colorIdx, markerIdx);
    for(int i=0; i<num; i++){
        double ratio_mode = y[i]/yBase[i];
        double variance = (y[i]+1.)*(y[i]+2.)/((yBase[i]+2.)*(yBase[i]+3.)) - (y[i]+1.)*(y[i]+1.)/((yBase[i]+2.)*(yBase[i]+2.)); // following Bayesian statistics for efficiency
        double sigma = sqrt(variance);

        double ratio_mean = (y[i]+1.)/(yBase[i]+2.); // <e> 
        double diffRatio = ratio_mode - ratio_mean;
        double errorH = sigma-diffRatio;
        double errorL = sigma+diffRatio;

        if(yBase[i] < 1){continue;}
        graph -> SetPoint(graph->GetN(), x[i], ratio_mode);
        graph -> SetPointError(graph->GetN()-1, 0., 0., errorL, errorH);
    }
}

void DrawingUtil::SetGraphError(int num, double* x, double* y, double* ex, double* ey, TString title, int cIdx, int colorIdx, int markerIdx)
{
    TGraphAsymmErrors* graph = SetGraph(title, cIdx, colorIdx, markerIdx);
    for(int i=0; i<num; i++){
        graph -> SetPoint(graph->GetN(), x[i], y[i]);
        graph -> SetPointError(graph->GetN()-1, ex[i], ex[i], ey[i], ey[i]);
    }
}

void DrawingUtil::SetGraphAsymError(int num, double* x, double* y, double* exl, double* exh, double* eyl, double* eyh, TString title, int cIdx, int colorIdx, int markerIdx)
{
    TGraphAsymmErrors* graph = SetGraph(title, cIdx, colorIdx, markerIdx);
    for(int i=0; i<num; i++){
        graph -> SetPoint(graph->GetN(), x[i], y[i]);
        graph -> SetPointError(graph->GetN()-1, exl[i], exh[i], eyl[i], eyh[i]);
    }
}

TGraphAsymmErrors* DrawingUtil::SetGraph(TString title, int cIdx, int colorIdx, int markerIdx)
{
    int graphNum = mGraph.size();
    int objNum = 0;
    for(int i=0; i<graphNum; i++){
        if(mGraph[i].base.cIdx == cIdx){
            objNum++;
        }
    }

    TGraphAsymmErrors* graph = new TGraphAsymmErrors();
    graph -> SetName(Form("tmpGraph_c%i_obj%i", cIdx, objNum));

    int color = (colorIdx == -1)? GetColor(objNum) : GetColor(colorIdx);
    graph -> SetLineColor(color);
    graph -> SetMarkerColor(color);
    graph -> SetLineWidth(1.5);
    graph -> SetMarkerSize(2);
    graph -> SetLineColor(color);
    graph -> SetMarkerColor(color);

    int marker = (markerIdx == -1)? GetMarker(objNum) : GetMarker(markerIdx);
    graph -> SetMarkerStyle(marker);

    InfoGraph graphInfo;
    graphInfo.graph = graph;
    graphInfo.base.Clear();
    graphInfo.base.title = title;
    graphInfo.base.cIdx = cIdx;
    graphInfo.base.objIdx = objNum;
    graphInfo.base.colorIdx = colorIdx;
    graphInfo.base.markerIdx = markerIdx;

    mGraph.push_back(graphInfo);
    return graph;
}

TGraphAsymmErrors* DrawingUtil::GetGraph(int cIdx, int objIdx)
{
    int graphNum = mGraph.size();
     for(int i=0; i<graphNum; i++){
        if(mGraph[i].base.cIdx == cIdx){
            if(mGraph[i].base.objIdx == objIdx){
                return mGraph[i].graph;
            }
        }
    }
    return 0; 
}

void DrawingUtil::SetText(bool isPersistant, TString text, int cIdx, double x, double y, double size, int font)
{
    InfoText textInfo;
    textInfo.Clear();
    textInfo.persistency = isPersistant;
    textInfo.text = text;
    textInfo.cIdx = cIdx;
    textInfo.x = x;
    textInfo.y = y;
    textInfo.size = size;
    textInfo.font = font;
    mLatex.push_back(textInfo);
}

void DrawingUtil::SetLegend(bool isPersistant, TObject* obj, TString text, int cIdx, TString opt)
{   
    InfoText textInfo;
    textInfo.Clear();
    textInfo.persistency = isPersistant;
    textInfo.text = text;
    textInfo.cIdx = cIdx;

    if(opt == ""){
        TString name = obj->GetName();
        name.ToUpper();
        if(name.Index("GRAPH") != -1){
            opt = "p";
        }
        else{
            opt = "l";
        }
    }

    textInfo.opt = opt;
    textInfo.obj = obj;
    mLegend.push_back(textInfo);
}

void DrawingUtil::SetDrawOption(bool isPersistant, TString opt, int cIdx)
{
    opt.ToUpper();
    opt.ReplaceAll(",", " ");
    opt.ReplaceAll(";", " "); 
    TObjArray *tokens = opt.Tokenize(" ");
    for(int i=0; i<tokens->GetEntries(); i++){
        TString option = ((TObjString *)tokens->At(i))->GetString();
        if(isPersistant){cIdx = -1;}
        if(GetDrawFlag(opt, cIdx)){continue;}
        mDrawOptArr.push_back(make_pair(cIdx, opt));
    }
}

int DrawingUtil::GetColor(int idx)
{
    int Palette1[10] = {2, 4, 8, 95, 51, 91, 66, 28, 14, 1};
    int PaletteRainbow[7] = {2, 94, 90, 8, 4, kBlue+2, 51};

    if(idx < 0){return -1;}
    if(mColorPaletteIdx == 0){
        if(idx < 10){
            return Palette1[idx];
        }
        if(idx >= 10){
            return Palette1[9];
        }
    }
    else if(mColorPaletteIdx >= 1){
        if(idx < 7){
            return PaletteRainbow[idx];
        }
        if(idx >= 7){
            return 1;
        }
    }
    return -1;
}
int DrawingUtil::GetMarker(int idx)
{
    int markerStyle1[10] = {20, 21, 22, 23, 29, 33, 34, 47, 49, 39};
    int markerStyle2[10] = {24, 25, 26, 32, 30, 27, 28, 46, 36, 38};

    if(idx < 0){return -1;}
    if(mColorPaletteIdx == 0){
        if(idx < 10){
            return markerStyle1[idx];
        }
        if(idx >= 10){
            return markerStyle2[idx-10];
        }
    }
    else if(mColorPaletteIdx >= 1){
        if(idx < 10){
            return markerStyle2[idx];
        }
        if(idx >= 10){
            return markerStyle1[idx-10];
        }
    }
    return -1;
}

void DrawingUtil::InitHist(int& cNum)
{
    cNum = -1;
    int histNum = mHist.size();
    for(int i=0; i<histNum; i++){
        if(cNum < mHist[i].base.cIdx+1){
            cNum = mHist[i].base.cIdx+1;
        }
    }
    if(cNum <= 0){cNum = 1;}

    for(int c=0; c<cNum; c++){
        InfoHist histInfo;

        int bins = 0;
        double minBin = 99999.;
        double maxBin = -99999.;
        double maxHeight = 0;
        TString histName;
        TString titleX;
        TString titleY;

        for(int i=0; i<histNum; i++){
            if(mHist[i].base.cIdx == c){
                if(GetDrawFlag("norm") || GetDrawFlag("norm", c)){
                    NormalizedTH1D(mHist[i].hist);
                }

                double nBinsX = mHist[i].hist -> GetNbinsX();
                double binLower = mHist[i].hist -> GetBinLowEdge(1);
                double binHigher = mHist[i].hist -> GetBinLowEdge(nBinsX) + mHist[i].hist->GetBinWidth(nBinsX);
                double maximum = mHist[i].hist -> GetMaximum();

                if(bins < nBinsX){bins = nBinsX;}
                if(minBin > binLower){minBin = binLower;}
                if(maxBin < binHigher){maxBin = binHigher;}
                if(maxHeight < maximum){
                    histName = mHist[i].hist->GetName();
                    maxHeight = maximum;
                }
                TString tmpName, tmpXTitle, tmpYTitle;
                bool validTitle = GetTitle(mHist[i].base.title, tmpName, tmpXTitle, tmpYTitle);

                if(validTitle){
                    titleX = tmpXTitle;
                    titleY = tmpYTitle;
                }
            }
        }
        histInfo.hist = new TH1D(Form("base_%s_TH1D_c%i", histName.Data(), c), "", bins, minBin, maxBin);
        histInfo.hist -> SetStats(0);

        histInfo.hist -> GetYaxis()->SetRangeUser(0.00011, maxHeight*1.4);
        if(GetDrawFlag("yLog") || GetDrawFlag("yLog", c)){histInfo.hist->GetYaxis()->SetRangeUser(0.00011, maxHeight*20);}
        if(GetDrawFlag("ratio") || GetDrawFlag("ratio", c)){
            if(titleY==""){titleY = "Ratio";}
            histInfo.hist->GetYaxis()->SetRangeUser(0., 1.5);
        }
        if(GetDrawFlag("yaxis") || GetDrawFlag("yaxis", c)){
            TString yaxisScale = (GetDrawFlag("yaxis"))? GetDrawValue("YAXIS") : (GetDrawFlag("yaxis", c)? GetDrawValue("YAXIS"): "1.4");
            double scale = yaxisScale.Atof();
            histInfo.hist->GetYaxis()->SetRangeUser(0.00011, maxHeight*scale);
        }
        if(GetDrawFlag("xaxis1") || GetDrawFlag("xaxis2")){
            TString xaxis1 = GetDrawValue("XAXIS1");
            TString xaxis2 = GetDrawValue("XAXIS2");
            double xRange1 = (GetDrawFlag("xaxis1"))? xaxis1.Atof() : minBin;
            double xRange2 = (GetDrawFlag("xaxis2"))? xaxis2.Atof() : maxBin;

            histInfo.hist->GetXaxis()->SetRangeUser(xRange1, xRange2);
        }
        if(GetDrawFlag("xaxis1", c) || GetDrawFlag("xaxis2", c)){
            
            TString xaxis1 = GetDrawValue("XAXIS1", c);
            TString xaxis2 = GetDrawValue("XAXIS2", c);

            cout << " c " << c << " | " << xaxis1 << " | " << xaxis2 << endl;
            double xRange1 = (GetDrawFlag("xaxis1", c))? xaxis1.Atof() : minBin;
            double xRange2 = (GetDrawFlag("xaxis2", c))? xaxis2.Atof() : maxBin;

            histInfo.hist->GetXaxis()->SetRangeUser(xRange1, xRange2);
        }

        if(GetDrawFlag("norm") || GetDrawFlag("norm", c)){
            if(titleY==""){
                TObjArray *tokens = titleX.Tokenize(" ");
                TString tmp = ((TObjString *)tokens->At(0))->GetString();
                titleY = Form("1/N dN/d%s", tmp.Data());
            }
        }
        else{
            if(titleY==""){
                titleY = "Counts";
            }
        }

        histInfo.hist -> SetTitle(Form(";%s;%s", titleX.Data(), titleY.Data()));
        histInfo.hist -> SetLineWidth(0);

        int xLabelNameNum = mLabelName.size();
        for(int b=0; b<xLabelNameNum; b++){
            int idx = mLabelName[b].first+1;
            TString name = mLabelName[b].second;
            if(idx < 1 || idx > histInfo.hist->GetNbinsX()){continue;}
            histInfo.hist -> GetXaxis()->SetBinLabel(idx, name);
        }   

        histInfo.base.cIdx = c;
        histInfo.base.objIdx = -1;
        mHist.push_back(histInfo);
    }
}

void DrawingUtil::InitHist2D(int& cNum)
{
    cNum = -1;
    int histNum = mHist2D.size();
    if(cNum <= 0){cNum = histNum;}

    for(int c=0; c<histNum; c++){
        mHist2D[c].hist -> SetStats(0);

        TString tmpName, tmpXTitle, tmpYTitle;
        bool validTitle = GetTitle(mHist2D[c].base.title, tmpName, tmpXTitle, tmpYTitle);
        mHist2D[c].hist -> SetTitle(Form(";%s;%s", tmpXTitle.Data(), tmpYTitle.Data()));

        int xLabelNameNum = mLabelName.size();
        for(int b=0; b<xLabelNameNum; b++){
            int idx = mLabelName[b].first+1;
            TString name = mLabelName[b].second;
            if(idx < 1 || idx > mHist2D[c].hist->GetNbinsX()){continue;}
            mHist2D[c].hist -> GetXaxis()->SetBinLabel(idx, name);
        }   
    }

}

void DrawingUtil::InitGraph(int& cNum)
{
    cNum = -1;
    int graphNum = mGraph.size();
    for(int i=0; i<graphNum; i++){
        if(cNum < mGraph[i].base.cIdx+1){
            cNum = mGraph[i].base.cIdx+1;
        }
    }
    if(cNum <= 0){cNum = 1;}

    for(int c=0; c<cNum; c++){
        double minX = 99999.;
        double maxX = -99999.;
        double minY = 99999.;
        double maxY = -99999.;
        int maxPointGraphIdx = 0;
        int tmpPointNum = 0;
        TString histName;
        TString titleX;
        TString titleY;

        for(int i=0; i<graphNum; i++){
            if(mGraph[i].base.cIdx == c){
                int pointNum = mGraph[i].graph -> GetN();
                for(int p=0; p<pointNum; p++){
                    double x, y;
                    mGraph[i].graph->GetPoint(p, x, y);
                    double exl = mGraph[i].graph->GetErrorXlow(p);
                    double exh = mGraph[i].graph->GetErrorXhigh(p);
                    double eyl = mGraph[i].graph->GetErrorYlow(p);
                    double eyh = mGraph[i].graph->GetErrorYhigh(p);
                    if(minX > (x-exl)){minX = (x-exl);}
                    if(maxX < (x+exh)){maxX = (x+exh);}
                    if(minY > (y-eyl)){minY = (y-eyl);}
                    if(maxY < (y+eyh)){maxY = (y+eyh);}
                }
                if(pointNum > tmpPointNum){
                    tmpPointNum = pointNum;
                    maxPointGraphIdx = i;
                }
                
                TString tmpName, tmpXTitle, tmpYTitle;
                bool validTitle = GetTitle(mGraph[i].base.title, tmpName, tmpXTitle, tmpYTitle);
                if(validTitle){
                    titleX = tmpXTitle;
                    titleY = tmpYTitle;
                }
            }
        }

        if(GetDrawFlag("ratio") || GetDrawFlag("ratio", c)){
            minY = 0.;
            maxY = 1.5;
            if(titleY == ""){
                titleY = "Ratio";
            }
        }
        if(GetDrawFlag("shift") || GetDrawFlag("shift", c)){
            double dx = (maxX-minX)/50.;
            double tmpIdx = 0.;
            for(int i=0; i<graphNum; i++){
                if(mGraph[i].base.cIdx == c){
                    int pointNum = mGraph[i].graph -> GetN();
                    for(int p=0; p<pointNum; p++){
                        double x, y;
                        mGraph[i].graph->GetPoint(p, x, y);
                        mGraph[i].graph->SetPoint(p, x+dx*tmpIdx, y);
                    }
                    tmpIdx += 1.;
                }
            }
        }

        InfoGraph graphInfo;
        graphInfo.graph = new TGraphAsymmErrors();
        graphInfo.graph -> SetName(Form("baseGraph_%s_%i", titleX.Data(), c));
        double diffX = (maxX-minX)/10.;
        graphInfo.graph -> SetPoint(graphInfo.graph->GetN(), minX-diffX, minY);
        for(int i=0;i<mGraph[maxPointGraphIdx].graph->GetN(); i++){
            double x, y;
            mGraph[maxPointGraphIdx].graph->GetPoint(i, x, y);
            graphInfo.graph -> SetPoint(graphInfo.graph->GetN(), x, y);
        }
        graphInfo.graph -> SetPoint(graphInfo.graph->GetN(), maxX, maxY);
        graphInfo.graph -> SetMarkerSize(0);
        graphInfo.graph -> SetLineWidth(0);
        graphInfo.graph -> SetTitle(Form(";%s;%s", titleX.Data(), titleY.Data()));
        graphInfo.base.cIdx = c;
        graphInfo.base.objIdx = -1;

        int xLabelNameNum = mLabelName.size();
        if(xLabelNameNum != 0){
            for(int i=0;i<mGraph[maxPointGraphIdx].graph->GetN(); i++){
                if(xLabelNameNum < i+1){break;}
                if(mLabelName[i].first != i){continue;}
                double x, y;
                mGraph[maxPointGraphIdx].graph->GetPoint(i, x, y);
                int bin = graphInfo.graph -> GetXaxis()->FindBin(x);
                graphInfo.graph -> GetXaxis()-> SetBinLabel(bin, mLabelName[i].second);
            }
            graphInfo.graph->GetXaxis()->SetLabelSize(0.05);
            graphInfo.graph->GetXaxis()->LabelsOption("h");
        }

        mGraph.push_back(graphInfo);
    }
}

void DrawingUtil::DrawText(int cNum)
{
    TLatex* latex = new TLatex();

    int latexNum = mLatex.size();
    double textStartX = 0.18;
    double textStartY = 0.925;
    for(int i=0; i<latexNum; i++){
        if(mLatex[i].persistency){
            TString text = mLatex[i].text;
            if(text == ""){continue;}
            double x = mLatex[i].x;
            double y = mLatex[i].y;
            if(x < 0.){x = textStartX;}
            if(y < 0.){y = textStartY;}
            if(mLatex[i].size > 0.){latex->SetTextSize(mLatex[i].size);}
            if(mLatex[i].font > 0.){latex->SetTextSize(mLatex[i].font);}
            latex -> DrawLatexNDC(textStartX, textStartY, Form("%s", text.Data()));
            textStartY -= 0.055;
        }
    }
    for(int i=0; i<latexNum; i++){
        if(!mLatex[i].persistency && mLatex[i].cIdx == -1){
            TString text = mLatex[i].text;
            if(text == ""){continue;}
            double x = mLatex[i].x;
            double y = mLatex[i].y;
            if(x < 0.){x = textStartX;}
            if(y < 0.){y = textStartY;}
            if(mLatex[i].size > 0.){latex->SetTextSize(mLatex[i].size);}
            if(mLatex[i].font > 0.){latex->SetTextSize(mLatex[i].font);}
            latex -> DrawLatexNDC(textStartX, textStartY, Form("%s", text.Data()));
            textStartY -= 0.055;
        }
    }
    for(int i=0; i<latexNum; i++){
        if(!mLatex[i].persistency && mLatex[i].cIdx == cNum){
            TString text = mLatex[i].text;
            if(text == ""){continue;}
            double x = mLatex[i].x;
            double y = mLatex[i].y;
            if(x < 0.){x = textStartX;}
            if(y < 0.){y = textStartY;}
            if(mLatex[i].size > 0.){latex->SetTextSize(mLatex[i].size);}
            if(mLatex[i].font > 0.){latex->SetTextSize(mLatex[i].font);}
            latex -> DrawLatexNDC(textStartX, textStartY, Form("%s", text.Data()));
            textStartY -= 0.055;
        }
    }
}

void DrawingUtil::DrawLegend(int cNum)
{
    TLegend* persistantLegend = new TLegend(0.64, 0.68, 0.888, 0.965);
    persistantLegend -> SetBorderSize(0);
    persistantLegend -> SetTextSize(0.035);
    int legNum = mLegend.size();
    int canvasLegNum = 0;

    for(int i=0; i<legNum; i++){
        if(mLegend[i].cIdx == -1 || mLegend[i].persistency){
            persistantLegend -> AddEntry(mLegend[i].obj, Form("%s", mLegend[i].text.Data()), mLegend[i].opt);
        }
        if(mLegend[i].cIdx == cNum && !mLegend[i].persistency){
            canvasLegNum++;
        }
    }

    if(canvasLegNum == 0){
        persistantLegend -> Draw("same");
    }
    else{
        TLegend* leg = new TLegend(0.63, 0.68, 0.9, 0.965);
        leg -> SetBorderSize(0);
        leg -> SetTextSize(0.035);
        for(int i=0; i<legNum; i++){
            if(mLegend[i].cIdx == cNum){
                leg -> AddEntry(mLegend[i].obj, Form("%s", mLegend[i].text.Data()), mLegend[i].opt);
            }
        }
        leg -> Draw("same");
    }
}

bool DrawingUtil::InitDrawOption(TString opt)
{
    opt.ToUpper();
    opt.ReplaceAll(",", " ");
    opt.ReplaceAll(";", " "); 
    TObjArray *tokens = opt.Tokenize(" ");
    for(int i=0; i<tokens->GetEntries(); i++){
        TString option = ((TObjString *)tokens->At(i))->GetString();
        if(GetDrawFlag(opt)){continue;}
        mDrawOptArr.push_back(make_pair(-1, option));
    }
    return true;
}

bool DrawingUtil::GetDrawFlag(TString opt, int cIdx)
{   
    opt.ToUpper();
    int num = mDrawOptArr.size();
    for(int i=0; i<num; i++){
        TString option = mDrawOptArr[i].second;
        int canvasIdx = mDrawOptArr[i].first;
        if(canvasIdx != cIdx){continue;}

        if(opt == "N" || opt == "NORM"){
            if(option == "N" || option == "NORM"){return true;}
        }
        if(opt == "YLOG" && option == "YLOG"){return true;}
        if(opt == "XLOG" && option == "XLOG"){return true;}
        if(opt == "ZLOG" && option == "ZLOG"){return true;}
        if(opt == "RATIO" && option == "RATIO"){return true;}
        if(opt == "SHIFT" && option == "SHIFT"){return true;}
        if(opt == "YAXIS" && option.Index("YAXIS") != -1){return true;}
        if(opt == "XAXIS1" && option.Index("XAXIS1") != -1){return true;}
        if(opt == "XAXIS2" && option.Index("XAXIS2") != -1){return true;}
    }
    return false;
}

TString DrawingUtil::GetDrawValue(TString opt, int cIdx)
{
    opt.ToUpper();
    int num = mDrawOptArr.size();
    for(int i=0; i<num; i++){
        TString option = mDrawOptArr[i].second;
        int canvasIdx = mDrawOptArr[i].first;
        if(canvasIdx != cIdx){continue;}

        option.ToUpper();
        if(opt == "YAXIS" && option.Index("YAXIS") != -1){
            TObjArray *tokens = option.Tokenize("=");
            return ((TObjString *)tokens->At(tokens->GetEntries()-1))->GetString();
        }
        if(opt == "XAXIS1" && option.Index("XAXIS1") != -1){
            TObjArray *tokens = option.Tokenize("=");
            return ((TObjString *)tokens->At(tokens->GetEntries()-1))->GetString();
        }
        if(opt == "XAXIS2" && option.Index("XAXIS2") != -1){
            TObjArray *tokens = option.Tokenize("=");
            return ((TObjString *)tokens->At(tokens->GetEntries()-1))->GetString();
        }
    }
    return "";
}

bool DrawingUtil::GetTitle(TString inputTitle, TString& name, TString& xTitle, TString& yTitle)
{
    if(inputTitle == "" || inputTitle.Index(";")==-1){return 0;}

    TObjArray *tokens = inputTitle.Tokenize(";");
    int entries = tokens -> GetEntries();
    if(entries==0 && inputTitle != ""){
        xTitle = inputTitle;
        return 1;
    }
    if(entries==1){
        xTitle = ((TObjString *)tokens->At(0))->GetString();
        return 1;
    }
    if(entries==2){
        xTitle = ((TObjString *)tokens->At(0))->GetString();
        yTitle = ((TObjString *)tokens->At(1))->GetString();
        return 1;
    }
    else if(entries==3){
        name = ((TObjString *)tokens->At(0))->GetString();
        xTitle = ((TObjString *)tokens->At(1))->GetString();
        yTitle = ((TObjString *)tokens->At(2))->GetString();
        return 1;
    }
    return 0;
}

void DrawingUtil::NormalizedTH1D(TH1D* hist, double scale)
{
    double entries = hist -> Integral();
    if(entries < 1){return;}
    if(scale > 0.){entries = scale;}
    for(int bin=0; bin<hist->GetNbinsX(); bin++){
        double entry = hist->GetBinContent(bin+1);
        double binWidth = hist->GetXaxis()->GetBinWidth(bin+1);
        hist->SetBinContent(bin+1, entry/binWidth/entries);
    }
}