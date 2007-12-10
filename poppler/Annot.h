//========================================================================
//
// Annot.h
//
// Copyright 2000-2003 Glyph & Cog, LLC
//
//========================================================================

#ifndef ANNOT_H
#define ANNOT_H

#ifdef USE_GCC_PRAGMAS
#pragma interface
#endif

class XRef;
class Gfx;
class Catalog;
class CharCodeToUnicode;
class GfxFont;
class GfxFontDict;
class FormWidget;
class PDFRectangle;

//------------------------------------------------------------------------
// AnnotBorder
//------------------------------------------------------------------------

class AnnotBorder {
public:
  enum AnnotBorderStyle {
    borderSolid,      // Solid
    borderDashed,     // Dashed
    borderBeveled,    // Beveled
    borderInset,      // Inset
    borderUnderlined, // Underlined
  };
	  
  virtual ~AnnotBorder();

  virtual double getWidth() { return width; }
  virtual int getDashLength() { return dashLength; }
  virtual double *getDash() { return dash; }
  virtual AnnotBorderStyle getStyle() { return style; }

protected:
  double width;
  int dashLength;
  double *dash;
  AnnotBorderStyle style;
};

//------------------------------------------------------------------------
// AnnotBorderArray
//------------------------------------------------------------------------

class AnnotBorderArray: public AnnotBorder {
public:
  AnnotBorderArray();
  AnnotBorderArray(Array *array);

  virtual double getHorizontalCorner() { return horizontalCorner; }
  virtual double getVerticalCorner() { return verticalCorner; }

protected:
  static const int DASH_LIMIT = 10; // implementation note 82 in Appendix H.
  double horizontalCorner;          // (Default 0)
  double verticalCorner;            // (Default 0)
  // double width;                  // (Default 1)  (inherited from AnnotBorder)
};

//------------------------------------------------------------------------
// AnnotBorderBS
//------------------------------------------------------------------------

class AnnotBorderBS: public AnnotBorder {
public:
  
  AnnotBorderBS();
  AnnotBorderBS(Dict *dict);

private:
  // double width;           // W  (Default 1)   (inherited from AnnotBorder)
  // AnnotBorderStyle style; // S  (Default S)   (inherited from AnnotBorder)
  // double *dash;           // D  (Default [3]) (inherited from AnnotBorder)
};

//------------------------------------------------------------------------
// AnnotColor
//------------------------------------------------------------------------

class AnnotColor {
public:

  enum AnnotColorSpace {
    colorTransparent = 0,
    colorGray        = 1,
    colorRGB         = 3,
    colorCMYK        = 4
  };
  
  AnnotColor();
  AnnotColor(Array *array);
  ~AnnotColor();

  AnnotColorSpace getSpace();
  double getValue(int i);

private:

  double *values;
  int length;
};

//------------------------------------------------------------------------
// AnnotBorderStyle
//------------------------------------------------------------------------

enum AnnotBorderType {
  annotBorderSolid,
  annotBorderDashed,
  annotBorderBeveled,
  annotBorderInset,
  annotBorderUnderlined
};

class AnnotBorderStyle {
public:

  AnnotBorderStyle(AnnotBorderType typeA, double widthA,
		   double *dashA, int dashLengthA,
		   double rA, double gA, double bA);
  ~AnnotBorderStyle();

  AnnotBorderType getType() { return type; }
  double getWidth() { return width; }
  void getDash(double **dashA, int *dashLengthA)
    { *dashA = dash; *dashLengthA = dashLength; }
  void getColor(double *rA, double *gA, double *bA)
    { *rA = r; *gA = g; *bA = b; }

private:

  AnnotBorderType type;
  double width;
  double *dash;
  int dashLength;
  double r, g, b;
};

//------------------------------------------------------------------------
// Annot
//------------------------------------------------------------------------

class Annot {
public:
  enum AnnotFlag {
    flagUnknown        = 0x0000,
    flagInvisible      = 0x0001,
    flagHidden         = 0x0002,
    flagPrint          = 0x0004,
    flagNoZoom         = 0x0008,
    flagNoRotate       = 0x0010,
    flagNoView         = 0x0020,
    flagReadOnly       = 0x0040,
    flagLocked         = 0x0080,
    flagToggleNoView   = 0x0100,
    flagLockedContents = 0x0200
  };

  Annot(XRef *xrefA, Dict *acroForm, Dict *dict, Catalog *catalog);
  Annot(XRef *xrefA, Dict *acroForm, Dict *dict, const Ref& aref, Catalog *catalog);
  ~Annot();
  GBool isOk() { return ok; }

  void draw(Gfx *gfx, GBool printing);

  // Get appearance object.
  Object *getAppearance(Object *obj) { return appearance.fetch(xref, obj); }
  GBool textField() { return isTextField; }

  GBool match(Ref *refA)
    { return ref.num == refA->num && ref.gen == refA->gen; }

  void generateFieldAppearance(Dict *field, Dict *annot, Dict *acroForm);

  double getXMin();
  double getYMin();

  double getFontSize() { return fontSize; }

  GooString *getType() { return type; }
  PDFRectangle *getRect() { return rect; }
  AnnotBorder *getBorder() { return border; }
  AnnotColor *getColor() { return color; }
  
private:
  void setColor(Array *a, GBool fill, int adjust);
  void drawText(GooString *text, GooString *da, GfxFontDict *fontDict,
		GBool multiline, int comb, int quadding,
		GBool txField, GBool forceZapfDingbats,
    GBool password=false);
  void drawListBox(GooString **text, GBool *selection,
		   int nOptions, int topIdx,
		   GooString *da, GfxFontDict *fontDict, GBool quadding);
  void getNextLine(GooString *text, int start,
		   GfxFont *font, double fontSize, double wMax,
		   int *end, double *width, int *next);
  void drawCircle(double cx, double cy, double r, GBool fill);
  void drawCircleTopLeft(double cx, double cy, double r);
  void drawCircleBottomRight(double cx, double cy, double r);
  void readArrayNum(Object *pdfArray, int key, double *value);
  // write vStr[i:j[ in appearBuf
  void writeTextString (GooString *text, GooString *appearBuf, int *i, int j, CharCodeToUnicode *ccToUnicode, GBool password); 

  void initialize (XRef *xrefA, Dict *acroForm, Dict *dict, Catalog *catalog);

  // required data
  PDFRectangle *rect;           // Rect

  // optional data
  GooString *contents;              // Contents
  Dict *pageDict;                   // P
  GooString *name;                  // NM
  GooString *modified;              // M
  Guint flags;                      // F (must be a 32 bit unsigned int)
  //Dict *appearDict;                 // AP (should be correctly parsed)
  Object appearance;     // a reference to the Form XObject stream
                         //   for the normal appearance
  GooString *appearState;           // AS
  int treeKey;                      // Struct Parent;
  Dict *optionalContent;            // OC

  XRef *xref;			// the xref table for this PDF file
  Ref ref;                      // object ref identifying this annotation
  FormWidget *widget;           // FormWidget object for this annotation
  GooString *type;              // annotation type
  GooString *appearBuf;
  AnnotBorder *border;          // Border, BS
  AnnotColor *color;            // C
  double fontSize; 
  GBool ok;
  GBool regen, isTextField;
  GBool isMultiline, isListbox;
  
  bool hasRef;
};

//------------------------------------------------------------------------
// Annots
//------------------------------------------------------------------------

class Annots {
public:

  // Build a list of Annot objects.
  Annots(XRef *xref, Catalog *catalog, Object *annotsObj);

  ~Annots();

  // Iterate through list of annotations.
  int getNumAnnots() { return nAnnots; }
  Annot *getAnnot(int i) { return annots[i]; }

  // (Re)generate the appearance streams for all annotations belonging
  // to a form field.
  void generateAppearances(Dict *acroForm);


private:
  void scanFieldAppearances(Dict *node, Ref *ref, Dict *parent,
			    Dict *acroForm);
  Annot *findAnnot(Ref *ref);

  Annot **annots;
  int nAnnots;
};

#endif
