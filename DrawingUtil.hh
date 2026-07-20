/////////////////////////////////////////////////////
//          ROOT based figure drawing uility       //
//  Author: Seunghwan Lee (zxzfdsa@gmail.com)      //
/////////////////////////////////////////////////////

#ifndef DrawingUtil_hh
#define DrawingUtil_hh

#include <cmath>
#include <iostream>
#include <fstream>
#include <vector>
#include <tuple>

#include "TSystem.h"
#include "TDirectory.h"
#include "TObject.h"
#include "TObjArray.h"
#include "TObjString.h"
#include "TString.h"
#include "TColor.h"
#include "TCanvas.h"
#include "TPad.h"
#include "TH1D.h"
#include "TH2D.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TGraphAsymmErrors.h"
#include "TLegend.h"
#include "TLatex.h"
#include "TLine.h"
#include "TAxis.h"

using namespace std;

class DrawingUtil
{
    struct InfoBase
    {
        TString title;
        int cIdx;
        int objIdx;
        int colorIdx;
        int markerIdx;

        void Clear(){
            title = "";
            cIdx = 0;
            objIdx = 0;
            colorIdx = -1;
            markerIdx = -1;
        };
    };

    struct InfoHist
    {
        TH1D* hist;
        InfoBase base;
    };

    struct InfoHist2D
    {
        TH2D* hist;
        InfoBase base;
    };

    struct InfoGraph
    {
        TGraphAsymmErrors* graph;
        InfoBase base;
    };

    struct InfoText
    {
        TString text;
        bool persistency;
        int cIdx;
        int objIdx;
        double x;
        double y;
        double size;
        double font;
        TObject* obj;
        TString opt;

        void Clear(){
            text = "";
            persistency = false;
            cIdx = 0;
            objIdx = 0;
            x = -1;
            y = -1;
            size = -1;
            font = -1;
            obj = 0;
            opt = "";
        };
    };
    
    public:
        DrawingUtil(TString figurePath="./");
        ~DrawingUtil();

        void Init();
        void Clear(TString opt=""); // [opt = all, text, leg]

        // ************ Drawing Options ******************
        // Global drawing option can be setted in DrawHist("option") or related functions. These option will be applied in all pads
        // If you want to set the option in specific pad, use the SetDrawOption() function.
        // --- Avaliable option list ---
        // n, norm          : hiostogram normalization
        // xlog, ylog, zlog : log drawing 
        // ratio            : ratio 0 to 1 
        // shift            : shift point of graph for each graphs in same canvas 
        // yaxis            : scale of draw range ex) yaxis=2.5 -> 2.5 times maximum hist bin contents
        // xaxis1, 2        : range of x axis, low range = xaxis1, high range = xaxis2, ex) xaxis1=10 
        // draw             : drawing option for ROOT, it can applied per objects
        // drawsub          : sub pad drawing option for ROOT, it can applied per objects
        // **********************************************

        void DrawHist(TString opt="");
        void DrawHistWithRatio(int baseIdx, TString opt="", TString yTitle="");
        void DrawHist2D(TString opt="");
        void DrawGraph(TString opt="");

        void SaveFigure(TString name);

        void SetCanvas(int divideNum, int xPadNum=-1, TString name="", double size=700.);
        void SetXLabelName(int i, TString name);

        void SetPalette(int idx){mColorPaletteIdx = idx;}
        void SetMarkerFill(int idx){mMarkerStyleIdx = idx;}

        void SetTH1DRatio(int num, double* y, double base, TString title="", int cIdx=-1, int colorIdx=-1, int markerIdx=-1);
        void SetTH1D(TH1D* hist, TString title="", int cIdx=-1, int colorIdx=-1, int markerIdx=-1);
        TH1D* GetTH1D(int cIdx, int objIdx);

        void SetTH2D(TH2D* hist, TString title="");
        TH2D* GetTH2D(int cIdx);

        TCanvas* GetCanvas(){return mCanvas;}

        void SetGraph(int num, double* x, double* y, TString title="", int cIdx=-1, int colorIdx=-1, int markerIdx=-1);
        void SetGraphRatio(int num, double* x, double* y, double* yBase, TString title="", int cIdx=-1, int colorIdx=-1, int markerIdx=-1);
        void SetGraphAsymRatio(int num, double* x, double* y, double* yBase, TString title="", int cIdx=-1, int colorIdx=-1, int markerIdx=-1);
        void SetGraphError(int num, double* x, double* y, double* ex, double* ey, TString title="", int cIdx=-1, int colorIdx=-1, int markerIdx=-1);
        void SetGraphAsymError(int num, double* x, double* y, double* exl, double* exh, double* eyl, double* eyh, TString title="", int cIdx=-1, int colorIdx=-1, int markerIdx=-1);
        TGraphAsymmErrors* SetGraph(TString title="", int cIdx=-1, int colorIdx=-1, int markerIdx=-1);
        TGraphAsymmErrors* GetGraph(int cIdx, int objIdx);

        void SetText(bool isPersistant, TString text, int cIdx=-1, double x=-1, double y=-1, double size=-1, int font=-1);
        void SetLegend(bool isPersistant, TObject* obj, TString text, int cIdx=-1, TString opt="");
        void SetDrawOption(bool isPersistant, TString opt, int cIdx=-1, int objIdx=-1);

        int GetColor(int idx);
        int GetMarker(int idx);

    private:
        void InitHist(int& cNum);
        void InitHist2D(int& cNum);
        void InitGraph(int& cNum);

        void DrawText(int cNum);
        void DrawLegend(int cNum);

        bool InitDrawOption(TString opt);
        bool GetDrawFlag(TString opt, int cIdx=-1, int objIdx=-1);
        TString GetDrawValue(TString opt, int cIdx=-1, int ojbIdx=-1);
        
        bool GetTitle(TString inputTitle, TString& name, TString& xTitle, TString& yTitle);

        TCanvas* mCanvas = nullptr;
        vector<tuple<int, int, TString> > mDrawOptArr;

        vector<InfoHist> mHist;
        vector<InfoHist2D> mHist2D;
        vector<InfoGraph> mGraph;

        vector<InfoText> mLatex;
        vector<InfoText> mLegend;

        int mColorPaletteIdx;
        int mMarkerStyleIdx;
        vector<pair<int, TString> > mLabelName;

        TString mFigurePath;
        int mCObjIdx;
        int mObjIdx;
};

#endif