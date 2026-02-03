#include <TROOT.h>
#include "DrawingUtil.hh"
#include "DrawingUtil.cc"

void example()
{
    TH1D* hist1D_1 = new TH1D("hist1D_1", "", 100, 0, 10);
    TH1D* hist1D_2 = new TH1D("hist1D_2", "", 100, 0, 100);
    TH2D* hist2D = new TH2D("hist2D", "", 100, 0, 100, 100, 0, 100);

    // Make sample events
    for(int event=0; event<10000; event++){
        double gaus1 = gRandom -> Gaus(5, 2.);
        double gaus2 = gRandom -> Gaus(20, 30.);
        double gaus3 = gRandom -> Gaus(60, 10.);
        hist1D_1 -> Fill(gaus1);
        hist1D_2 -> Fill(gaus2);
        hist1D_2 -> Fill(gaus3);
        hist2D -> Fill(gaus2, gaus3);
    }

    // DrwaingUtil class example 
    DrawingUtil* draw = new DrawingUtil("./figure");
    draw -> Clear(); // all, leg, text
    draw -> SetCanvas(2);

    // Legend and Latex
    bool isGlobal = true;
    draw -> SetText(isGlobal, "Text1");
    draw -> SetText(false, "Text2", 1);
    draw -> SetLegend(isGlobal, hist1D_1, "hist1");

    // TH1D drawing
    int CanvasIdx = 0;
    draw -> SetTH1D(hist1D_1, "NO put hist title; x title; y title", CanvasIdx);
    CanvasIdx += 1;
    draw -> SetLegend(false, hist1D_2, "hist2", CanvasIdx);
    draw -> SetTH1D(hist1D_2, "NO put hist title; x title; y title", CanvasIdx);
    draw -> DrawHist(); // n : normalization
    draw -> SaveFigure("hist1D_savename");

    // TH1D with sub Pad for fraction
    draw -> Clear("leg");
    draw -> SetLegend(isGlobal, hist1D_1, "hist1");
    draw -> SetLegend(isGlobal, hist1D_2, "hist2");
    draw -> SetCanvas(1);
    draw -> SetTH1D(hist1D_1, "NO put hist title; x title; y title", 0); // objIdx = 0
    draw -> SetTH1D(hist1D_2, ";x;y", 0); // objIdx = 1
    int baseObjIdx = 0;
    draw -> DrawHistWithRatio(baseObjIdx, "n");
    draw -> SaveFigure("histFraction_savename");

    // TH2D drawing
    draw -> Clear("leg"); 
    draw -> SetCanvas(1);
    draw -> SetTH2D(hist2D, ";xx;yy");
    draw -> DrawHist2D();
    draw -> SaveFigure("hist2D_savename");

    

}

