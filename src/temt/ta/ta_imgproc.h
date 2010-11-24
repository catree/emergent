// Copyright, 1995-2007, Regents of the University of Colorado,
// Carnegie Mellon University, Princeton University.
//
// This file is part of The Emergent Toolkit
//
//   This library is free software; you can redistribute it and/or
//   modify it under the terms of the GNU Lesser General Public
//   License as published by the Free Software Foundation; either
//   version 2.1 of the License, or (at your option) any later version.
//   
//   This library is distributed in the hope that it will be useful,
//   but WITHOUT ANY WARRANTY; without even the implied warranty of
//   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//   Lesser General Public License for more details.

#ifndef ta_imgproc_h
#define ta_imgproc_h

// image processing operations

// there are multiple levels of filtering and image processing, ordered in this file as:
// -- taImage, Canvas -- basic data and drawing on images
// -- basic image filtering: DoG, Gabor, etc
// -- taImgProc -- static container class with various image processing operations
// -- specific parameterizations for DoG and V1 levels of image processing, representing a
//	region of the retina (DoGRegionSpec, V1RegionSpec)
// -- an overall image processing object containing multiple regions, providing full
//	final coverage of the image processing from program user perspective:
//	RetinaProc, DoGRetinaProc, V1RetinaProc

#include "ta_math.h"
#include "ta_program.h"
#include "ta_geometry.h"
#include "ta_datatable.h"
#include "fun_lookup.h"

#if defined(TA_GUI) && !defined(__MAKETA__)
# include <QImage>
# include <QPainter>
# include <QPainterPath>
#endif

class QPainterPath;	// #IGNORE
class QBrush;		// #IGNORE

#ifdef TA_OS_WIN
# ifdef LoadImage
#   undef LoadImage
# endif
#endif

class TA_API taImage : public taNBase {
  // #STEM_BASE ##CAT_Image represents an image in TA accessible way (wrapper for QT's QImage)
  INHERITED(taNBase)
public:
#ifdef TA_GUI
  QImage&  	GetImage() { return q_img; }
  // get the underlying qt image
  void  	SetImage(const QImage& img) { q_img = img; }
  // #CAT_Image set the underlying qt image
#endif
  virtual bool	LoadImage(const String& fname);
  // #BUTTON #CAT_File #FILE_DIALOG_LOAD #EXT_png,jpg load the image from given file name (leave file name blank to get a chooser)
  virtual bool	SaveImage(const String& fname);
  // #BUTTON #CAT_File #FILE_DIALOG_SAVE #EXT_png,jpg save the image to given file name (leave file name blank to get a chooser) -- uses extension to determine format

  virtual float	GetPixelGrey_float(int x, int y);
  // #CAT_Image get the pixel value as a greyscale for given coordinates
  virtual bool	GetPixelRGB_float(int x, int y, float& r, float& g, float& b);
  // #CAT_Image get the pixel value as floating point RGB values for given coordinates

  bool	ImageToMatrix_grey(float_Matrix& grey_data);
  // #CAT_Data convert image to greyscale Matrix floating point image data (alias for ImageToGrey_float): note that this uses standard matrix convention where 0,0 = bottom left of image, not top left..
  bool	ImageToMatrix_rgb(float_Matrix& rgb_data);
  // #CAT_Data convert image to RGB Matrix floating point image data (alias for ImageToMatrix_rgb) -- img_data is 3 dimensional with 3rd dim = r,g,b: note that this uses standard matrix convention where 0,0 = bottom left of image, not top left..

  bool	ImageFromMatrix_grey(const float_Matrix& grey_data);
  // #CAT_Data convert from greyscale Matrix floating point image data to this image: note that this uses standard matrix convention where 0,0 = bottom left of image, not top left..
  bool	ImageFromMatrix_rgb(const float_Matrix& rgb_data);
  // #CAT_Data convert from RGB Matrix floating point image data to this image -- img_data is 3 dimensional with 3rd dim = r,g,b: note that this uses standard matrix convention where 0,0 = bottom left of image, not top left..

  virtual bool	ImageToDataCell(DataTable* dt, Variant col, int row);
  // #BUTTON #CAT_Data set image to datatable cell indexed by col (name or number) and row -- uses cell dimensionality and type -- only amount that fits in cell is copied. row = -1 = last row
  virtual bool	ImageFromDataCell(DataTable* dt, Variant col, int row);
  // #BUTTON #CAT_Data set image from data in datatable cell indexed by col (name or number) and row -- uses cell dimensionality and type -- only amount that fits in cell is copied. row = -1 = last row
  virtual bool	ConfigDataColName(DataTable* dt, const String& col_nm, ValType val_type, 
				  bool rgb = true);
  // #CAT_Data configure data column with given name (if it doesn't exist, it is created) to represent current image -- if rgb is false, then a greyscale image is configured (2d)

  virtual bool	ScaleImage(float sx, float sy, bool smooth=true);
  // #BUTTON #CAT_Image scale image by given normalized scaling factors in each dimension
  virtual bool	RotateImage(float norm_deg, bool smooth=true);
  // #BUTTON #CAT_Image rotate image by given normalized degrees (1 = 360deg)
  virtual bool	TranslateImage(float move_x, float move_y, bool smooth=true);
  // #BUTTON #CAT_Image translate image by given normalized factors (-1 = all the way left, +1 = all the way right, etc)

  virtual bool	GetImageSize(int& width, int& height);
  // #CAT_Image get size of current image
  virtual bool	SetImageSize(int width, int height);
  // #BUTTON #CAT_Image set size of current image -- if currently same size, then returns false and nothing happens; otherwise, a new image data structure of given size is created, using ARGB32 format


  ////////////////////////////////////
  // Obsolete Interfaces: do not use:
  
  virtual bool	ImageToDataCellName(DataTable* dt, const String& col_nm, int row);
  // #CAT_zzzObsolete set image to datatable cell indexed by col name and row number -- uses cell dimensionality and type -- only amount that fits in cell is copied. row = -1 = last row
  virtual bool	ImageToGrey_float(float_Matrix& grey_data) { return ImageToMatrix_grey(grey_data); }
  // #CAT_zzzObsolete convert image to greyscale floating point image data: note that this uses standard matrix convention where 0,0 = bottom left of image, not top left..
  virtual bool	ImageToRGB_float(float_Matrix& rgb_data)  { return ImageToMatrix_rgb(rgb_data); }
  // #CAT_zzzObsolete convert image to RGB floating point image data -- img_data is 3 dimensional with 3rd dim = r,g,b: note that this uses standard matrix convention where 0,0 = bottom left of image, not top left..


  void	Copy_(const taImage& cp);
  TA_BASEFUNS(taImage);
protected:
#ifdef TA_GUI
  QImage	q_img;
#endif
private:
  void 	Initialize();
  void	Destroy() { };
};

class TA_API taCanvas : public taImage {
  // ##CAT_Data provides a mechanism for drawing onto an image buffer
INHERITED(taImage)
public:
  enum CoordType {
    PIXELS,			// raw pixels
    NORMALIZED,			// normalized 0-1 values
  };

  CoordType 	coord_type;	// type of coordinates to use in drawing (0,0 is lower left)

  virtual void 	InitCanvas();
  // #CAT_Canvas initialize the canvas -- MUST be called prior to drawing!
  inline bool	CheckInit() {
    TestError(!m_init, "CheckInit", "Must call InitCanvas before drawing!");
    return m_init;
  }
  virtual void	EraseRGBA(float r, float g, float b, float a);
  // #CAT_Canvas erase image to given rgba color
  virtual void	EraseName(const String& name);
  // #CAT_Canvas erase image to given color name

  virtual void	Point(float x, float y);
  // #CAT_Draw 
  virtual void	Line(float x1, float y1, float x2, float y2);
  // #CAT_Draw 
  virtual void 	Rect(float l, float b, float r, float t);
  // #CAT_Draw 
  virtual void 	Circle(float x, float y, float r);
  // #CAT_Draw 
  virtual void 	Ellipse(float x, float y, float rx, float ry);
  // #CAT_Draw 
  virtual void 	FillRect(float l, float b, float r, float t);
  // #CAT_Draw 
  virtual void 	FillCircle(float x, float y, float r);
  // #CAT_Draw 
  virtual void 	FillEllipse(float x, float y, float rx, float ry);
  // #CAT_Draw 
  
  virtual void 	NewPath();
  // #CAT_Path start a new path (must have done DrawPath or FillPath on any prior paths first!)
  virtual void 	MoveTo(float x, float y);
  // #CAT_Path move current path position to given coordinate without drawing -- will create a new path if not yet initialized
  virtual void 	LineTo(float x, float y);
  // #CAT_Path 
  virtual void 	CurveTo(float x, float y, float x1, float y1, float x2, float y2);
  // #CAT_Path draw Bezier curve to given x,y point, using x1,y1 and x2,y2 as control points that determine curvature
  virtual void 	DrawPath();
  // #CAT_Path draw the current path using current pen brush and close it -- a NewPath or MoveTo is required to start a new path
  virtual void 	FillPath();
  // #CAT_Path fill the current path using current fill brush and close it -- a NewPath or MoveTo is required to start again
  virtual void 	DrawFillPath();
  // #CAT_Path draw outline and fill the current path using current pen and fill brush and close it -- a NewPath or MoveTo is required to start again
  virtual void 	DeletePath();
  // #CAT_Path delete the current path -- called by DrawPath and FillPath -- use to remove accidental path if needed

  virtual void 	TextLeft(const String& txt, float x, float y);
  // #CAT_Text draw text left justified
  virtual void 	TextCenter(const String& txt, float x, float y);
  // #CAT_Text draw text centered
  virtual void 	TextRight(const String& txt, float x, float y);
  // #CAT_Text  draw text right justified
  virtual void	SetFont(const String& font_name, int point_size=-1, int weight=-1, bool italic=false);
  // #CAT_Text set font for text drawing operations

  virtual void 	ClipRect(float l, float b, float r, float t);
  // #CAT_Clip

  virtual void	PenColorRGBA(float r, float g, float b, float a);
  // #CAT_Draw values are from 0-1 in proportion to amount of each given color, a is alpha transparency value, 0=transparent, 1=opaque
  virtual void	PenColorHSVA(float h, float s, float v, float a);
  // #CAT_Draw h=hue (0-360), s=saturation (0-1), v=value/brightness (0-1), a is alpha transparency value, 0=transparent, 1=opaque
  virtual void	PenColorCMYKA(float c, float m, float y, float k, float a);
  // #CAT_Draw values are from 0-1 in proportion to amount of each given color, a is alpha transparency value, 0=transparent, 1=opaque
  virtual void	PenColorName(const String& name);
  // #CAT_Draw 
  virtual void	PenWidth(float width);
  // #CAT_Draw 
  virtual void	FillColorRGBA(float r, float g, float b, float a);
  // #CAT_Draw values are from 0-1 in proportion to amount of each given color, a is alpha transparency value, 0=transparent, 1=opaque
  virtual void	FillColorHSVA(float h, float s, float v, float a);
  // #CAT_Draw h=hue (0-360), s=saturation (0-1), v=value/brightness (0-1), a is alpha transparency value, 0=transparent, 1=opaque
  virtual void	FillColorCMYKA(float c, float m, float y, float k, float a);
  // #CAT_Draw values are from 0-1 in proportion to amount of each given color, a is alpha transparency value, 0=transparent, 1=opaque
  virtual void	FillColorName(const String& name);
  // #CAT_Draw 

  void	Copy_(const taCanvas& cp);
  TA_BASEFUNS(taCanvas);
protected:
#ifdef TA_GUI
  QPainter	q_painter;
  QPainterPath*  cur_path;
  QBrush	fill_brush;
  bool		m_init;
#endif
private:
  void Initialize();
  void Destroy();
};

/////////////////////////////////////////////////////////////
//		Threading code

#include "ta_thread.h"

class ImgProcThreadBase;

// this is the standard function call taking the thread number int value
// all threaded functions MUST use this call signature!
#ifdef __MAKETA__
typedef void* ThreadImgProcCall;
#else
typedef taTaskMethCall2<ImgProcThreadBase, void, int, int> ThreadImgProcCall;
typedef void (ImgProcThreadBase::*ThreadImgProcMethod)(int, int);
#endif

class ImgProcCallThreadMgr;

class TA_API ImgProcCallTask : public taTask {
INHERITED(taTask)
public:
  ThreadImgProcCall* img_proc_call;	// method to call on the object

  override void run();
  // runs specified chunk of computation

  ImgProcCallThreadMgr* mgr() { return (ImgProcCallThreadMgr*)owner->GetOwner(); }

  TA_BASEFUNS_NOCOPY(ImgProcCallTask);
private:
  void	Initialize();
  void	Destroy();
};

class TA_API ImgProcCallThreadMgr : public taThreadMgr {
  // #INLINE thread manager for ImgProcCall tasks -- manages threads and tasks, and coordinates threads running the tasks
INHERITED(taThreadMgr)
public:
  int		min_units;	// #MIN_1 #NO_SAVE NOTE: not saved -- initialized from user prefs.  minimum number of computational units of work required to use threads at all -- if less than this number, all will be computed on the main thread to avoid threading overhead which may be more than what is saved through parallelism, if there are only a small number of things to compute.
  int		nibble_chunk;	// #MIN_1 #DEF_8 #NO_SAVE NOTE: not saved -- initialized from user prefs.  how many units does each thread grab to process while nibbling?  Too small a value results in increased contention and inefficiency, while too large a value results in poor load balancing across processors.

  QAtomicInt	nibble_i;	// #IGNORE current nibble index -- atomic incremented by working threads to nibble away the rest..
  int		n_cmp_units;	// #IGNORE number of compute units to perform -- max of the nibbling..

  ImgProcThreadBase*	img_proc() 	{ return (ImgProcThreadBase*)owner; }

  override void		Run(ThreadImgProcCall* img_proc_call, int n_cmp_un);
  // #IGNORE run given function, splitting n_cmp_units computational units evenly across the available threads
  
  TA_BASEFUNS_NOCOPY(ImgProcCallThreadMgr);
private:
  void	Initialize();
  void	Destroy();
};

class TA_API ImgProcThreadBase : public taNBase {
  // #VIRT_BASE ##CAT_Image base class for image-processing code that uses threading -- defines a basic interface for thread calls to deploy filtering or other intensive computations
  INHERITED(taNBase)
public:
  ImgProcCallThreadMgr threads; // #CAT_Threads parallel threading of image processing computation

  void 	Initialize() { };
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(ImgProcThreadBase);
};

////////////////////////////////////////////////////////////////////
//		Basic Filters: DoG, Gabor...

class TA_API DoGFilter : public taNBase {
  // #STEM_BASE #INLINE ##CAT_Image defines a difference-of-gaussians (center minus surround or "mexican hat") filter that highlights contrast in an image
  INHERITED(taNBase)
public:
#ifdef __MAKETA__
  String	name;		// #HIDDEN_INLINE name of object
#endif

  int		filter_width;	// half-width of the filter (typically 2 * off_sigma)
  int		filter_size;	// #READ_ONLY size of the filter: 2 * width + 1
  float		on_sigma;	// width of the narrower central 'on' gaussian
  float		off_sigma;	// width of the wider surround 'off' gaussian (typically 2 * on_sigma)
  int		spacing;	// spacing between filters -- should be same as on_sigma
  bool		circle_edge;	// #DEF_true cut off the filter (to zero) outside a circle of radius filter_width -- makes the filter more radially symmetric
  float_Matrix	on_filter;	// #READ_ONLY #NO_SAVE #NO_COPY on-gaussian 
  float_Matrix	off_filter;	// #READ_ONLY #NO_SAVE #NO_COPY off-gaussian (values are positive)
  float_Matrix	net_filter;	// #READ_ONLY #NO_SAVE #NO_COPY net overall filter -- on minus off

  inline float	FilterPoint(int x, int y, float img_val) {
    return img_val * net_filter.FastEl(x+filter_width, y+filter_width);
  }
  // #CAT_DoGFilter apply filter at given x,y point (-filter_width..filter_width) given image value (which can be either luminance or color contrast
  float		InvertFilterPoint(int x, int y, float act) {
    return 0.5f * act * net_filter.FastEl(x+filter_width, y+filter_width + 0.5f);
  }
  // #CAT_DoGFilter invert filter at given x,y point with net activation, returning image value -- this is probably not very accurate

  virtual void	RenderFilter(float_Matrix& on_flt, float_Matrix& off_flt, float_Matrix& net_flt);
  // #CAT_DoGFilter render filter into matrix
  virtual void	UpdateFilter();
  // #CAT_DoGFilter make our personal filter (RenderFilter(filter)) according to current params

  virtual void	GraphFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter gaussian into data table and generate a graph
  virtual void	GridFilter(DataTable* disp_data, bool reset = true);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter gaussian into data table and generate a grid view (reset any existing data first)

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(DoGFilter);
protected:
  void	UpdateAfterEdit_impl();
};

class TA_API GaborFilter : public taNBase {
  // #STEM_BASE ##CAT_Image defines a gabor filter (gaussian times a plane sine wave) that highlights lines/edges in an image
INHERITED(taNBase)
public:
  enum GaborParam {
    CTR_X,
    CTR_Y,
    ANGLE,
    PHASE,
    FREQ,
    LENGTH,
    WIDTH,
    AMP,
  };

#ifdef __MAKETA__
  String	name;		// #HIDDEN_INLINE name of object
#endif
  int		x_size;		// overall size of the filtered region
  int		y_size;		// overall size of the filtered region
  float		ctr_x;		// center in x coord
  float		ctr_y;		// center in y coord
  float		angle;		// angle of sine wave in 2-d space (in radians)
  float		phase;		// phase of sine wave wrt the center of the gaussian (radians)
  float		freq;		// frequency of the sine wave
  float		length;		// length of the gaussian perpendicular to the wave direction
  float		width;		// width of the gaussian in the wave direction
  float		amp;		// amplitude (maximum value)
  float_Matrix	filter;		// #READ_ONLY #NO_SAVE #NO_COPY our filter

  virtual float	Eval(float x, float y);
  // #CAT_GaborFilter evaluate gabor function for given coordinates
  virtual void	RenderFilter(float_Matrix& flt);
  // #CAT_GaborFilter render filter into matrix
  virtual void	UpdateFilter();
  // #CAT_GaborFilter make our personal filter (RenderFilter(filter)) according to current params

  virtual float	GetParam(GaborParam param);
  // #CAT_GaborFilter get particular parameter value

  virtual void	GraphFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter into data table and generate a graph from it
  virtual void	GridFilter(DataTable* disp_data, bool reset = true);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter into data table and generate a grid view of it (reset an existing data first)

  virtual void	OutputParams(ostream& strm = cerr);
  // #CAT_GaborFilter output current parameter values to stream

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(GaborFilter);
};


class TA_API MotionGaborFilter : public taNBase {
  // #STEM_BASE ##CAT_Image defines a gabor filter oriented in space and time (gaussian times a plane sine wave) that moving highlights lines/edges in an image.
INHERITED(taNBase)
public:
  enum MotionGaborParam {
    CTR_X,
    CTR_Y,
    CTR_T,
    SPAT_ANGLE,
    TIME_ANGLE,
    PHASE,
    FREQ,
    FREQ_T,
    WIDTH,
    LENGTH,
    WIDTH_T,
    AMP,
  };

  int		x_size;		// overall size of the filtered region
  int		y_size;		// overall size of the filtered region
  int		t_size;		// overall size of the filtered region
  float		ctr_x;		// center in x coord
  float		ctr_y;		// center in y coord
  float		ctr_t;		// center in y coord

  float		spat_angle;	// angle of sine wave in 2-d space (in radians)
  float		time_angle;	// angle of sine wave in 2-d time (in radians)

  float		phase;		// phase of sine wave wrt the center of the gaussian (radians)
  float		freq;		// frequency of the sine wave
  float		freq_t;		// frequency of the sine wave

  float		width;		// width of the gaussian in the wave direction
  float		length;		// width of the gaussian in the wave direction
  float		width_t;	// width of the gaussian in the wave direction
  float		amp;		// amplitude (maximum value)
  bool		use_3d_gabors;
  float_Matrix  filter;		// #READ_ONLY #NO_SAVE #NO_COPY our filter

  virtual float	Eval(float x, float y, float t);
  // #CAT_GaborFilter evaluate gabor function for given coordinates
  virtual void	RenderFilter(float_Matrix& flt);
  // #CAT_GaborFilter render filter into matrix
  virtual void	UpdateFilter();
  // #CAT_GaborFilter make our personal filter (RenderFilter(filter)) according to current params

  virtual float	GetParam(MotionGaborParam param);
  // #CAT_GaborFilter get particular parameter value

  virtual void	GraphFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter into data table and generate a graph from it
  virtual void	GridFilter(DataTable* disp_data, bool reset = true);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filter into data table and generate a grid view of it (reset an existing data first)

  virtual void	OutputParams(ostream& strm = cerr);
  // #CAT_GaborFilter output current parameter values to stream

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(MotionGaborFilter);
};

class TA_API GaborFitter : public GaborFilter {
  // ##CAT_Image fits a gabor filter from image data
INHERITED(GaborFilter)
public:
  float		fit_dist;	// #READ_ONLY #SHOW fit distance

  virtual float ParamDist(const GaborFilter& oth);
  // #CAT_GaborFilter return euclidian distance between parameters for this spec and the other one

//   virtual float	FitData(float_Matrix& data_vals, bool use_cur_vals = false);
  // find best-fitting parameters for given data.  use_cur_vals = use current values as initial reasonable guess (skip first-pass search)
//   virtual float	FitData_firstpass(float_Matrix& data_vals);
//   virtual float	SquaredDist(float_Matrix& data_vals);

//   virtual float	TestFit();	// #BUTTON test the fitting function
 
  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(GaborFitter);
};


///////////////////////////////////////////////////////////////
// 	taImageProc

class TA_API taImageProc : public taNBase {
  // #STEM_BASE ##CAT_Image basic image processing operations; images are either rgb = 3 dimensional matrix with 3rd dim = 3 or greyscale (2 dim matrix)
  INHERITED(taNBase)
public:

  enum EdgeMode {		// how to deal with image edges
    CLIP,			// just hard clip edges, nothing fancy
    BORDER,			// render/preserve a 1 pixel border around everything
    WRAP,			// wrap the image around to the other side: no edges!
  };

  static bool	GetBorderColor_float(float_Matrix& img_data, float& r, float& g, float& b);
  // #CAT_Render get the average color around a 1 pixel border region of the image -- if grey-scale image, r,g,b are all set to the single grey value
    static bool	GetBorderColor_float_rgb(float_Matrix& img_data, float& r, float& g, float& b);
    // #CAT_Render get the average color around a 1 pixel border region of the image -- specifically for rgb image
    static bool	GetBorderColor_float_grey(float_Matrix& img_data, float& grey);
    // #CAT_Render get the average color around a 1 pixel border region of the image -- specifically for grey scale image

  static bool	RenderBorder_float(float_Matrix& img_data);
  // #CAT_Render make a uniform border 1 pixel wide around image, containing average value for that border region in original image: this value is what gets filled in when image is translated "off screen"

  static bool	FadeEdgesToBorder_float(float_Matrix& img_data, int fade_width = 8);
  // #CAT_Render fade edges of image uniformly to the border color, over fade_width pixels

  static bool	RenderOccluderBorderColor_float(float_Matrix& img_data,
						float llx, float lly, float urx, float ury);
  // #CAT_Render render an occluder rectangle of given normalized size (ll = lower left corner (0,0 = farthest ll), ur = upper right (1,1 = farthest ur) using the border color

  static void	GetWeightedPixels_float(float coord, int size, int* pc, float* pw);
  // #IGNORE helper function: get pixel coordinates (pc[0], pc[1]) with norm weights (pw[0], [1]) for given floating coordinate coord

  static bool   TranslateImagePix_float(float_Matrix& xlated_img, float_Matrix& orig_img, 
					int move_x, int move_y, EdgeMode edge=BORDER);
  // #CAT_Transform #MENU_BUTTON #MENU_ON_Transform move (translate) image by pixel move_x, move_y factors
  static bool   TranslateImage_float(float_Matrix& xlated_img, float_Matrix& orig_img, 
				   float move_x, float move_y, EdgeMode edge=BORDER);
  // #CAT_Transform #MENU_BUTTON #MENU_ON_Transform move (translate) image by normalized move_x, move_y factors: 1 = center of image moves to right/top edge, -1 center moves to bottom/left
  static bool	RotateImage_float(float_Matrix& rotated_img, float_Matrix& orig_img,
				  float rotation, EdgeMode edge=BORDER);
  // #CAT_Transform #MENU_BUTTON #MENU_ON_Transform rotate the image: rotation = normalized 0-1 = 0-360 degrees 
  static bool	ScaleImage_float(float_Matrix& scaled_img, float_Matrix& orig_img, float scale,
				 EdgeMode edge=BORDER);
  // #CAT_Transform #MENU_BUTTON #MENU_ON_Transform change the size of the image by normalized scaling factor (either rgb=3 dim or grey=2 dim)
  static bool   CropImage_float(float_Matrix& crop_img, float_Matrix& orig_img, 
				int crop_width, int crop_height, EdgeMode edge=BORDER);
  // #CAT_Transform #MENU_BUTTON #MENU_ON_Transform crop image to given size (-1 = use original image size), centered on the center of the image; border color of original image is used to fill in missing values

  static bool	TransformImage_float(float_Matrix& xformed_img, float_Matrix& orig_img,
				     float move_x=0.0f, float move_y=0.0f, float rotate=0.0f,
				     float scale=1.0f, int crop_width=-1, int crop_height=-1,
				     EdgeMode edge=BORDER);
  // #CAT_Transform #MENU_BUTTON #MENU_ON_Transform Transform an image by translation, rotation, scaling, and cropping, as determined by parameters (calls above functions; only if needed; if crop < 0 then no cropping); does RenderBorder for each step to preserve uniform background color
  static bool	SampleImageWindow_float(float_Matrix& out_img, float_Matrix& in_img,
					int win_width=320, int win_height=320,
					float ctr_x=0.5f, float ctr_y=0.5f,
					float rotate=0.0f, float scale=1.0f,
					EdgeMode edge=BORDER);
  // #CAT_Transform #MENU_BUTTON #MENU_ON_Transform Sample a window of given width and height centered on given normalized coordinate location in input image, with scaling and rotation (in normalized 0-1 units) as specified
  
  static bool	AttentionFilter(float_Matrix& mat, float radius_pct);
  // #CAT_Filter #MENU_BUTTON #MENU_ON_Filter apply an "attentional" filter to the matrix data: outside of radius, values are attenuated in proportion of squared distance outside of radius (r_sq / dist_sq) -- radius_pct is normalized proportion of maximum half-size of image (e.g., 1 = attention bubble extends to furthest edge of image; only corners are attenuated)

  static bool	BlobBlurOcclude(float_Matrix& img, float pct_occlude,
				float circ_radius=0.05, float gauss_sig=0.05,
				EdgeMode edge=BORDER, bool use_border_clr=true);
  // #CAT_Noise #MENU_BUTTON #MENU_ON_Noise occlude the image (in place -- affects the img matrix itself) by blurring gaussian blobs into the image -- blobs are uniform circles with gaussian blur around the edges -- radius and gaussian sigma are in image width normalized units (e.g., .05 = 5% of the width of the image), pct_occlude is proportion of total image to occlude computed as a proportion of (1/gauss_sig)^2 as the total number of different blobs possible -- actual blob locations are drawn at random, so complete occlusion is not ensured even at 1, though there is an extra 2x factor at 1 to really try to occlude as completely as possible -- if use_border_clr, then the 1 pixel border around image provides the color for all the blobs -- otherwise color is weighted local average using same gaussian blobo kernel
  
  static bool	AdjustContrast(float_Matrix& img, float new_contrast);
  // #CAT_Noise #MENU_BUTTON #MENU_ON_Adjust the contrast of the image (in place -- affects the img matrix itself) using new_contrast as a scalar. Holds background color constant. Both new_contrast is in range [0 1]

  override String 	GetTypeDecoKey() const { return "DataTable"; }
  TA_BASEFUNS_NOCOPY(taImageProc);
private:
  void 	Initialize();
  void	Destroy();
};


////////////////////////////////////////////////////////////////////////////
//	Collections of filters and parameters for applying to images

////////////////////////////////////////////////////////////////////
//		Base Class 

class TA_API VisRegionParams : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image basic params for a visual region
INHERITED(taOBase)
public:
  enum Ocularity {		// ocularity configuration
    MONOCULAR,			// monocular -- only one eye
    BINOCULAR,			// binocular -- both eyes
  };
  enum Region {			// retinal region
    FOVEA,
    PARAFOVEA,
    PERIPHERY,
  };
  enum Resolution {		// level of resolution
    HI_RES,
    MED_RES,
    LOW_RES,
    VLOW_RES,
  };
  enum Color {			// color processing
    MONOCHROME,			// just luminance on/off
    COLOR,			// has luminance on/off plus 4 color contrasts: R, G, B, Y vs. other two colors
  };
  enum EdgeMode {		// how to deal with edges in the filter inputs
    CLIP,			// hard clip edges -- attempts to ensure that no clipping is necessary by making filters fit within inputs, but image input is clipped
    WRAP,			// wrap the image and any subsequent stages of filtering around the edges -- no edges!
  };

  Ocularity	ocularity;	// whether two eyes or only one is present
  Region	region;		// retinal region represented by this filter 
  Resolution	res;		// level of resolution represented by this filter (can use enum or any other arbitrary rating scale -- just for informational/matcing purposes)
  Color		color;		// what level of color information to process
  EdgeMode	edge_mode;	// how to deal with edges throughout the processing cascade -- the edge_mode for the raw image transformations are in the overall RetinaProc, and are not automatically sync'd with this (they can be different)
  float		renorm_thr;	// #DEF_1e-05 threshold for the max filter output value to max-renormalize filter outputs such that the max is 1 -- below this value, consider the input to be blank and do not renorm

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(VisRegionParams);
};

class TA_API VisRegionSizes : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image basic size values for a visual region -- defines the size of visual image that is presented to the filters
INHERITED(taOBase)
public:
  TwoDCoord	retina_size;	// overall size of the retina -- defines size of images that are processed by these filters -- scaling etc typically used to fit image to retina size
  TwoDCoord	border;		// border around retina that we don't process -- for non-WRAP mode, typically a 1 pixel background color border is retained in the input image processing, so this should be subtracted -- also for non-WRAP mode, good to ensure that this is >= than 1/2 of the width of the filters being applied
  TwoDCoord	input_size;	// #READ_ONLY #SHOW size of input region in pixels that is actually filtered -- retina_size - 2 * border

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(VisRegionSizes);
protected:
  void	UpdateAfterEdit_impl();
};

class TA_API VisRegionSpecBase : public ImgProcThreadBase {
  // #STEM_BASE ##CAT_Image base class for specifying a visual image region to be filtered according to a set of filters -- used as part of overall RetinaProc processing object -- takes image bitmap inputs and produces filter activation outputs
INHERITED(ImgProcThreadBase)
public:
  enum ColorChannel {		// indicator of which color channel to filter on
    LUMINANCE,			// just raw luminance (monochrome / black white)
    RED_CYAN,			// red vs. cyan (G + B)
    GREEN_MAGENTA,		// green vs. magenta (R + B)
    BLUE_YELLOW,		// blue vs. yellow (R + G)
  };
  enum Eye {
    LEFT,			
    RIGHT,
  };
  enum RenormMode {		// how to renormalize output of filters
    NO_RENORM,			// do not renormalize
    LIN_RENORM,			// linear renormalization -- divide all filter output by max value
    LOG_RENORM,			// log renormalize -- take the log of all filter output with max = 1 multiplier -- this tends to work the best
  };
  enum DataSave {		// #BITS how to save data to the data table (computation happens on internal table)
    NO_SAVE 	= 0,	 	// #NO_BIT don't save data for this component
    SAVE_DATA	= 0x0001, 	// save data to the data table, by default with all features in subgroups within one matrix (unless SEP_MATRIX is checked)
    SEP_MATRIX	= 0x0002,	// each feature is saved in a separate 2d map, instead of a common subgroup within a single matrix
    ONLY_GUI	= 0x0004,	// only save when the gui is active -- for data that is for debugging and visualization purposes only, not presented to a network etc
    SAVE_DEBUG	= 0x0008,	// save extra debugging-level internal computation data relevant to this processing step
  };
  enum SaveMode {		// how to add new data to the data table
    NONE_SAVE,			// don't save anything at all -- overrides any more specific save guys and prevents any addition or modification to the data table
    FIRST_ROW,			// always overwrite the first row -- does EnforceRows(1) if rows = 0
    ADD_ROW,			// always add a new row and write to that, preserving a history of inputs over time -- should be reset at some interval!
  };

  enum OutMatIdx {		// matrix indexes for matrix output storage
    FEAT_X,			// feature sub-group x
    FEAT_Y,			// feature sub-group y
    IMG_X,			// filters for image, x
    IMG_Y,			// filters for image, y
    TIME,			// time is out index
  };

  DataTableRef	data_table;	// data table for saving filter results for viewing and applying to networks etc
  SaveMode	save_mode;	// how to add new data to the data table
  DataSave	image_save;	// how to save the input image(s) for each filtering step
  VisRegionParams region;	// basic parameters for the region
  VisRegionSizes  input_size;	// size of the visual input image, including any borders etc
  int		motion_frames;	// #MIN_0 how many frames of image information are to be retained for extracting motion signals -- 0 = no motion, 3 = typical for motion

  virtual bool 	Init();
  // #BUTTON initialize everything to be ready to start filtering -- calls InitFilters, InitOutMatrix, InitDataTable

  virtual bool	FilterImage(float_Matrix* right_eye_image, float_Matrix* left_eye_image = NULL);
  // main interface: filter input image(s) (if ocularity = BINOCULAR, must pass both images, else left eye is ignored) -- saves results in local output vectors, and data table if specified -- increments the time index if motion filtering

  virtual bool	InvertFilters();
  // #BUTTON invert filters, using data that currently exists in the local filter storage -- copy from other source to last stage of filtering _out matrix to invert something -- results are saved in output data table image output -- can copy from there as needed

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(VisRegionSpecBase);
protected:
  override void	UpdateAfterEdit_impl();

  // cache of args for current function call
  float_Matrix* cur_img_r;	// cur right eye image arg -- only valid during filter call
  float_Matrix* cur_img_l;	// cur left eye image arg -- only valid during filter call
  float_Matrix* cur_img;	// cur image -- only valid during filter call
  float_Matrix* cur_out;	// cur output buffer -- only valid during filter call
  CircMatrix*	cur_circ;	// current circular buffer index
  bool		rgb_img;	// is current image rgb?
  bool		wrap;		// whether edge_mode == WRAP

  float_Matrix cur_img_grey;	// greyscale version of color image, if input is rgb
  float_Matrix cur_img_rc;	// RED vs. CYAN version of color image, if input is rgb
  float_Matrix cur_img_gm;	// GREEN vs. MAGENTA version of color image, if input is rgb
  float_Matrix cur_img_by;	// BLUE vs. YELLOW version of color image, if input is rgb

  virtual void UpdateGeom();
  // update all geometry info -- called by UAE

  inline float&	MatMotEl(float_Matrix* fmat, int fx, int fy, int imx, int imy, int motdx) {
    if(motion_frames <= 1)
      return fmat->FastEl(fx, fy, imx, imy);
    else
      return fmat->FastEl(fx, fy, imx, imy, motdx);
  }
  // convenience for accessing matrix element with either motion or not depending on setting

  virtual bool NeedsInit();
  // test to see if the system needs to be initialized -- just check if things fit with the current computed geometries -- be sure to test for both eyes if binocular..
  virtual bool InitFilters();
  // initialize the filters -- overload for derived types and call parent
  virtual bool InitOutMatrix();
  // initialize data output matrcies to fit output of filters
  virtual bool InitDataTable();
  // initialize data table to fit data saving as configured

  virtual bool FilterImage_impl();
  // implementation of filtering -- assumes cur_img_x args are set and everything is checked
  virtual void IncrTime();
  // increment time one step -- move the CircMatrix indexes

  virtual bool ColorRGBtoCMYK(float_Matrix& img);
  // convert RGB color image to Cyan vs. Red, Magenta vs Green, Yellow vs. Blue, and Grey separate images, which are what should be then used for filtering (stored in cur_img_xx float matrix's)
  virtual float_Matrix* GetImageForChan(ColorChannel cchan);
  // get the appropriate cur_img_* guy for given color channel

  virtual bool RenormOutput_Frames(RenormMode mode, float_Matrix* out, CircMatrix* circ);
  // renormalize output of filters after filtering -- for output having motion frames
  virtual bool RenormOutput_NoFrames(RenormMode mode, float_Matrix* out);
  // renormalize output of filters after filtering -- for output without motion frames

  virtual bool ImageToTable(DataTable* dtab, float_Matrix* right_eye_image,
				     float_Matrix* left_eye_image = NULL);
  // send current input image(s)e step of dog output to data table for viewing
    virtual bool ImageToTable_impl(DataTable* dtab, float_Matrix* img, const String& col_sufx);
    // send current input image(s)e step of dog output to data table for viewing

  virtual bool InvertFilters_impl();
  // implementation of inverse filtering
};

class TA_API VisRegionSpecBaseList : public taList<VisRegionSpecBase> {
  // ##CAT_Image a list of visual region image processing filters
INHERITED(taList<VisRegionSpecBase>)
public:

  virtual VisRegionSpecBase* FindRetinalRegion(VisRegionParams::Region reg);
  // find first spec with given retinal region
  virtual VisRegionSpecBase* FindRetinalRes(VisRegionParams::Resolution res);
  // find first spec with given resolution
  virtual VisRegionSpecBase* FindRetinalRegionRes(VisRegionParams::Region reg,
					      VisRegionParams::Resolution res);
  // find first spec with given retinal region and resolution (falls back to res then reg if no perfect match)

  TA_BASEFUNS_NOCOPY(VisRegionSpecBaseList);
private:
  void	Initialize() 		{ SetBaseType(&TA_VisRegionSpecBase); }
  void 	Destroy()		{ };
};


////////////////////////////////////////////////////////////////////
//		Basic Retinal Processing: DoG's 

class TA_API DoGRegionSpec : public VisRegionSpecBase {
  // #STEM_BASE ##CAT_Image specifies a region of Difference-of-Gaussian retinal filters -- used as part of overall RetinaProc processing object -- takes image bitmap inputs and produces filter activation outputs
INHERITED(VisRegionSpecBase)
public:
  DoGFilter	dog_specs;	// Difference of Gaussian retinal filter specification
  RenormMode	dog_renorm;	// #DEF_LOG_RENORM how to renormalize the output of filters
  DataSave	dog_save;	// how to save the DoG outputs for the current time step in the data table
  XYNGeom	dog_feat_geom; 	// #READ_ONLY #SHOW size of one 'hypercolumn' of features for DoG filtering -- x axis = 2 = on/off, y axis = color channel: 0 = monochrome, 1 = red/cyan, 2 = green/magenta, 3 = blue/yellow (2 units total for monochrome, 8 total for color)
  TwoDCoord	dog_img_geom; 	// #READ_ONLY #SHOW size of dog-filtered image output -- number of hypercolumns in each axis to cover entire output -- this is completely determined by retina_size, border and dog_spacing parameters

  CircMatrix	dog_circ_r; 	// #NO_SAVE #NO_COPY #READ_ONLY circular buffer indexing for time
  CircMatrix	dog_circ_l; 	// #NO_SAVE #NO_COPY #READ_ONLY circular buffer indexing for time

  float_Matrix	dog_out_r;	// #READ_ONLY #NO_SAVE output of the dog filter processing for the right eye -- [feat.x][feat.y][img.x][img.y][time] -- time is optional depending on motion_frames
  float_Matrix	dog_out_l;	// #READ_ONLY #NO_SAVE output of the dog filter processing for the left eye -- [feat.x][feat.y][img.x][img.y][time] -- time is optional depending on motion_frames

  virtual String GetDoGFiltName(int filt_no);
  // get name for each filter channel (0-5) = on;off;rvc;gvm;bvy;yvb

  virtual void	GraphDoGFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable #LABEL_GraphDoGFilter plot the filter difference-of-gaussians into data table and generate a graph
  virtual void	GridDoGFilter(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable #LABEL_GridDoGFilter plot the filter difference-of-gaussians into data table and generate a grid view

  virtual void	PlotSpacing(DataTable* disp_data, bool reset = true);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable #ARGC_1 plot the arrangement of the filters (centers) in the data table using given value, and generate a grid view

  // todo: add CheckConfig!!

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(DoGRegionSpec);
protected:
  void	UpdateAfterEdit_impl();

  override void	UpdateGeom();

  override bool NeedsInit();
  override bool InitFilters();
  override bool InitOutMatrix();
  override bool InitDataTable();

  override bool	FilterImage_impl();
  override void IncrTime();

  virtual bool DoGFilterImage(float_Matrix* image, float_Matrix* out, CircMatrix* circ);
  // implementation of DoG filtering for a given image and output, circ index -- manages threaded calls to _thread version
  virtual void DoGFilterImage_thread(int dog_idx, int thread_no);
  // threaded routine for actually filtering given index of dog
  virtual bool RenormOutput_Frames(RenormMode mode, float_Matrix* out, CircMatrix* circ);
  // renormalize output of filters after filtering -- for output having motion frames
  virtual bool RenormOutput_NoFrames(RenormMode mode, float_Matrix* out);
  // renormalize output of filters after filtering -- for output without motion frames

  virtual bool DoGOutputToTable(DataTable* dtab);
  // send current time step of dog output to data table for viewing
    virtual bool DoGOutputToTable_impl(DataTable* dtab, float_Matrix* out, CircMatrix* circ,
				       const String& col_sufx);
    // send current time step of dog output to data table for viewing

  override bool InvertFilters_impl();
  virtual bool	DoGInvertFilter(float_Matrix* out, CircMatrix* circ, const String& col_sufx);
  // implementation of DoG inverse filtering that actually does the heavy lifting
};


////////////////////////////////////////////////////////////////////
//		V1 Processing -- basic RF's

class TA_API V1KwtaSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image k-winners-take-all dynamics for v1 image processing stages -- based on Leabra dynamics with asymptotic activation settling, using the gelin version of the NXX1 activation function  -- see Leabra docs for more info on various parameters (LeabraUnitSpec, LeabraLayerSpec)
INHERITED(taOBase)
public:
  bool		on;	// is kwta active for this stage of processing?
  int		gp_k;	// #CONDSHOW_ON_on number of active units within a group (hyperocolumn) of features
  float		gp_g;	// #CONDSHOW_ON_on #DEF_0.02;0.1 gain on sharing of group-level inhibition with other unit groups throughout the layer -- spreads inhibition throughout the layer based on strength of competition happening within each unit group -- sets an effective minimum activity level
  float		kwta_pt; // #CONDSHOW_ON_on #DEF_0.5 k-winner-take-all inhibitory point value between avg of top k and remaining bottom units (uses KWTA_AVG_BASED -- 0.5 is gelin default)
  float		gain;	 // #CONDSHOW_ON_on #DEF_40 gain on the NXX1 activation function (based on g_e - g_e_thr value -- i.e. the gelin version of the function)
  float		nvar;	 // #CONDSHOW_ON_on #DEF_0.01 noise variance to convolve with XX1 function to obtain NOISY_XX1 function -- higher values make the function more gradual at the bottom
  float		g_bar_l; // #CONDSHOW_ON_on #DEF_0.1;0.3 leak current conductance value -- determines neural response to weak inputs -- a higher value can damp the neural response
  float		g_bar_e; // #READ_ONLY #NO_SAVE excitatory conductance multiplier -- multiplies filter input value prior to computing membrane potential -- general target is to have max excitatory input = .5, so with 0-1 normalized inputs, this value is automatically set to .5
  float		e_rev_e; // #READ_ONLY #NO_SAVE excitatory reversal potential -- automatically set to default value of 1 in normalized units
  float		e_rev_l; // #READ_ONLY #NO_SAVE leak and inhibition reversal potential -- automatically set to 0.3 (gelin default)
  float		thr;	 // #READ_ONLY #NO_SAVE firing threshold -- automatically set to default value of .5 (gelin default) 

  virtual bool	Compute_Kwta(float_Matrix& inputs, float_Matrix& outputs,
			     float_Matrix& gc_i_mat);
  // main interface: given the input matrix (raw output of filtering step), compute output activations via kwta function, using gc_i_mat to store inhib values per feature group, and g_i_tmp_mat as a temporary computation matrix for local spreading inhibition

  virtual void	Compute_Inhib(float_Matrix& inputs, float_Matrix& gc_i_mat);
  // inhib impl: given the input matrix (raw output of filtering step), compute inhibitory currrents for each feature group, stored in gc_i matrix
  virtual void	Compute_Act(float_Matrix& inputs, float_Matrix& outputs, float_Matrix& gc_i_mat);
  // activation impl: compute activations from inputs and computed inhibition

  virtual void	Compute_Inhib_IThr(float_Matrix& inputs, float_Matrix& gc_i_mat, 
				   float_Matrix& ithrs);
  // alternative compute inhib that computes all ithresh values into given array for subsequent usage in other inhibition computations
  virtual void	Compute_All_IThr(float_Matrix& inputs, float_Matrix& ithrs);
  // compute all ithresh values and store into temporary array -- for alternative activation schmes

  inline float 	Compute_IThresh(float gc_e) {
    return ((gc_e * e_rev_sub_thr_e + gbl_e_rev_sub_thr_l) / (thr_sub_e_rev_i));
  } 
  // compute inhibitory threshold value -- amount of inhibition to put unit right at firing threshold membrane potential

  inline float 	Compute_EThresh(float gc_i) {
    return ((gc_i * e_rev_sub_thr_i + gbl_e_rev_sub_thr_l) / (thr_sub_e_rev_e));
  } 
  // compute excitatory threshold value -- amount of excitation to put unit right at firing threshold membrane potential

  inline float 	Compute_EqVm(float gc_e, float gc_i) {
    float new_v_m = ((gc_e * e_rev_e + gber_l + (gc_i * e_rev_l)) / (gc_e + g_bar_l + gc_i));
    return new_v_m;
  }
  // compute equilibrium membrane potential from excitatory (gc_e) and inhibitory (gc_i) input currents (gc_e = raw filter value, gc_i = inhibition computed from kwta) -- in normalized units (e_rev_e = 1), and inhib e_rev_i = e_rev_l
  
  inline float 	Compute_ActFmVm_nxx1(float val_sub_thr) {
    float new_act;
    if(val_sub_thr <= nxx1_fun.x_range.min) {
      new_act = 0.0f;
    }
    else if(val_sub_thr >= nxx1_fun.x_range.max) {
      val_sub_thr *= gain;
      new_act = val_sub_thr / (val_sub_thr + 1.0f);
    }
    else {
      new_act = nxx1_fun.Eval(val_sub_thr);
    }
    return new_act;
  }

  inline float Compute_ActFmIn(float gc_e, float gc_i) {
    // gelin version:
    float g_e_thr = Compute_EThresh(gc_i);
    return Compute_ActFmVm_nxx1(gc_e - g_e_thr);
  }

  virtual void	CreateNXX1Fun();  // #CAT_Activation create convolved gaussian and x/x+1 

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V1KwtaSpec);
  //protected:

#ifndef __MAKETA__
  FunLookup	nxx1_fun;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Activation convolved gaussian and x/x+1 function as lookup table
  FunLookup	noise_conv;	// #HIDDEN #NO_SAVE #NO_INHERIT #CAT_Activation gaussian for convolution
#endif
  float		gber_l;	 // #READ_ONLY #NO_SAVE g_bar_l * e_rev_l -- just a compute time saver
  float		e_rev_sub_thr_e;// #READ_ONLY #NO_SAVE #HIDDEN e_rev_e - thr -- used for compute_ithresh
  float		e_rev_sub_thr_i;// #READ_ONLY #NO_SAVE #HIDDEN e_rev_i - thr -- used for compute_ithresh
  float		gbl_e_rev_sub_thr_l;// #READ_ONLY #NO_SAVE #HIDDEN g_bar_l * (e_rev_l - thr) -- used for compute_ithresh
  float		thr_sub_e_rev_i;// #READ_ONLY #NO_SAVE #HIDDEN thr - e_rev_i used for compute_ithresh
  float		thr_sub_e_rev_e;// #READ_ONLY #NO_SAVE #HIDDEN thr - e_rev_e used for compute_ethresh

 protected:
  void 	UpdateAfterEdit_impl();
};

class TA_API V1GaborSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image params for v1 simple cells as gabor filters: 2d Gaussian envelope times a sinusoidal plane wave -- by default produces 2 phase asymmetric edge detector filters
INHERITED(taOBase)
public:
  float		gain;		// #DEF_2 overall gain multiplier applied after gabor filtering -- only relevant if not using renormalization (otherwize it just gets renormed away)
  int		n_angles;	// #DEF_4 number of different angles encoded -- currently only 4 is supported
  int		filter_size;	// #DEF_6;12;18;24 size of the overall filter -- number of pixels wide and tall for a square matrix used to encode the filter -- filter is centered within this square
  int		spacing;	// how far apart to space the centers of the gabor filters -- 1 = every pixel, 2 = every other pixel, etc -- high-res should be 1, lower res can be increments therefrom
  float		wvlen;		// #DEF_6;12;18;24 (values = filter_size work well) wavelength of the sine waves -- number of pixels over which a full period of the wave takes place (computation adds a 2 PI factor to translate into pixels instead of radians)
  float		gauss_sig_len;	// #DEF_0.25:0.3 gaussian sigma for the length dimension (elongated axis perpendicular to the sine waves) -- normalized as a function of filter_size
  float		gauss_sig_wd;	// #DEF_0.2 gaussian sigma for the width dimension (in the direction of the sine waves) -- normalized as a function of filter_size
  float		phase_off;	// #DEF_0;1.5708 offset for the sine phase -- can make it into a symmetric gabor by using PI/2 = 1.5708
  bool		circle_edge;	// #DEF_true cut off the filter (to zero) outside a circle of diameter filter_size -- makes the filter more radially symmetric

  virtual void	RenderFilters(float_Matrix& fltrs);
  // generate filters into the given matrix, which is formatted as: [filter_size][filter_size][n_angles]

  virtual void	GridFilters(float_Matrix& fltrs, DataTable* disp_data, bool reset = true);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot the filters into data table and generate a grid view (reset any existing data first)

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V1GaborSpec);
protected:
  void 	UpdateAfterEdit_impl();
};

class TA_API V1sNeighInhib : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image neighborhood inhibition for V1 simple layer -- inhibition spreads along orthogonal orientations to line tuning, to eliminate redundant reps of the same underlying edge
INHERITED(taOBase)
public:
  bool		on;		// #DEF_true whether to use neighborhood orientation-sl
  int		inhib_d; 	// #CONDSHOW_ON_on #DEF_1 distance of neighborhood for inhibition to apply to same feature in neighboring locations spreading out on either side along the orthogonal direction relative to the orientation tuning -- 0 = do not compute
  float		inhib_g;	// #CONDSHOW_ON_on #DEF_0.8 gain factor for feature-specific inhibition from neighbors -- this proportion of the neighboring feature's threshold-inhibition value (used in computing kwta) is spread among neighbors according to inhib_d distance

  int		tot_ni_len;	// #READ_ONLY total length of neighborhood inhibition stencils = 2 * neigh_inhib_d + 1

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V1sNeighInhib);
protected:
  void 	UpdateAfterEdit_impl();
};

class TA_API V1MotionSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image params for v1 motion coding by simple cells
INHERITED(taOBase)
public:
  int		n_speeds;	// #DEF_1 for motion coding, number of speeds in each direction to encode separately -- speeds are 1, 2, 4, 8, etc and tuning_width is proportional to speed -- only applicable if motion_frames > 1
  int		tuning_width;	// #DEF_1 additional width of encoding around the trajectory for the target speed -- allows for some fuzziness in encoding -- effective value is multiplied by speed, so it gets fuzzier as speed gets higher
  float		gauss_sig;	// #DEF_0.8 gaussian sigma for weighting the contribution of extra width guys -- normalized by effective tuning_width

  int		tot_width;	// #READ_ONLY total width = 1 + 2 * tuning_width

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V1MotionSpec);
protected:
  void 	UpdateAfterEdit_impl();
};

class TA_API V1BinocularSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image params for v1 binocular cells
INHERITED(taOBase)
public:
  int		n_disps;	// #DEF_1 number of different disparities encoded in each direction away from the focal plane (e.g., 1 = 1 near and 1 far)
  int		disp_off;	// #DEF_10 offset from corresponding location for each disparity step
  int		tuning_width; 	// #DEF_5 for non-focal disparities: additional width of encoding around target offset disparity -- allows for some fuzziness in encoding -- last disparity on near/far has extra end_width too
  int		end_width;	// #DEF_10 extra tuning width on the ends (adds beyond the tuning width), to extend out and capture all reasonable disparities
  float		gauss_sig; 	// #DEF_0.7 gaussian sigma for weighting the contribution of tuning width -- normalized by tuning_width -- last disparity on near/far ends does not come back down
  int		n_matches;	// #DEF_5 number of best-fitting disparity matches to keep per point in initial processing step
  int		win_half_sz;	// #DEF_3 aggregation window half size -- window of feature samples this wide on all sides of current location is used to aggregate the best match over that local region
  float		opt_thr;	// #DEF_0.01 optimization threshold -- if source value is below this value, disparity is not computed and result is zero
  float		good_thr;	// #DEF_0.5 threshold on normalized average absolute distance over features to be considered a good match -- can then be added to the matches list
  float		cnt_thr;	// #DEF_2 in selecting the best disparity offset, threshold for number of matches to even consider as a candidate for best offset
  float		pct_thr;	// #DEF_0.1 in selecting the best disparity offset, threshold for selecting based on percent of total matches at a given disparity for the disparity with the greatest number of matches -- below this value, the one with the minimum distance is selected regardless of number of matches
  float		neigh_wt;	// #DEF_0.5 contribution of neighboring disparity offsets to current one in window aggregation procedure

  int		tot_disps;	// #READ_ONLY total number of disparities coded: 1 + 2 * n_disps
  int		max_width;	// #READ_ONLY maximum total width (1 + 2 * tuning_width for symmetric, + end_width for ends)
  int		max_off;	// #READ_ONLY maximum possible offset -- furthest point out in any of the stencils
  int		tot_offs;	// #READ_ONLY 1 + 2 * max_off
  int		win_sz;		// #READ_ONLY window full size = 1 + 2*win_half_sz
  int		win_area;	// #READ_ONLY total number of elements in the full square window = win_sz * win_sz

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V1BinocularSpec);
protected:
  void 	UpdateAfterEdit_impl();
};

class TA_API V1DispSpreadSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image params for disparity spreading in the v1 binocular cells -- spreads along orientations
INHERITED(taOBase)
public:
  bool		on;		// #DEF_false whether to do disparity spreading
  int		dsp_iters;	// #CONDSHOW_ON_on #DEF_10;0 number of iterations of spreading operation to spread consistency among local disparity codings
  float		dsp_gain;	// #CONDSHOW_ON_on #DEF_0.2 gain of influence of neighboring values in spreading disparities
  float		v1b_mix;	// #CONDSHOW_ON_on #DEF_0.5 how much overall disp spreading result influences the v1b disparity weightings in the end -- values < 1 preserve some of the original ambiguity of the bottom-up signal, as a hedge against potentially imperfect grouping results
  int 		neigh_width;	// #CONDSHOW_ON_on #DEF_4 neighborhood width -- how many locations on either side of each point to integrate neighborhood values over
  float		gauss_sig; 	// #CONDSHOW_ON_on #DEF_1 gaussian sigma for weighting the neighbors -- more distant ones are more weakly weighted

  int		tot_width;	// #READ_ONLY #NO_SAVE #HIDDEN neigh_width * 2 + 1
  float		v1b_mix_c;	// #READ_ONLY 1 - v1b_mix

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V1DispSpreadSpec);
protected:
  void 	UpdateAfterEdit_impl();
};


class TA_API V1ComplexSpec : public taOBase {
  // #STEM_BASE #INLINE #INLINE_DUMP ##CAT_Image params for v1 complex cells, which integrate over v1 simple or binocular
INHERITED(taOBase)
public:
  int		len_sum_len;	// #DEF_1 length (in pre-grouping of v1s/b rf's) beyond rf center (aligned along orientation of the cell) to integrate length summing -- this is a half-width, such that overall length is 1 + 2 * len_sum_len
  int		end_stop_dist;	// #DEF_2 end-stop distance factor -- how far away from the central point should we look for opposing orientations
  float		es_adjang_wt;	// #DEF_0.2 weight for adjacent angles in the end stop computation -- adjacent angles are often activated for edges that are not exactly aligned with the gabor angles, so they can result in false positives
  float		es_gain;	// #DEF_1.2 gain factor applied only to end stop outputs -- these have a more stringent criterion and thus can benefit from an additional mulitplier to put on same level compared to the others (len sum, v1s_max)
  float		gauss_sig;	// #DEF_0.8 gaussian sigma for spatial rf -- weights the contribution of more distant locations more weakly
  float		nonfocal_wt;	// #DEF_0.8 how much weaker are the non-focal binocular disparities compared to the focal one (which has a weight of 1)
  bool		pre_gp4;	// #DEF_true use a 4x4 pre-grouping of v1s or v1b features prior to computing subsequent steps (end stop, length sum, etc) -- pre grouping reduces the computational cost of subsequent steps, and also usefully makes it more robust to minor variations -- size must be even due to half-overlap for spacing requirement, so 4x4 is only size that makes sense -- if this is selected, then v1s_specs.line_len should be 4 as well, though 5 is possible (just extends lines over the edge a bit)
  TwoDCoord	spat_rf;	// integrate over this many spatial locations (uses MAX operator over gaussian weighted filter matches at each location) in computing the response of the v1c cells -- produces a larger receptive field -- operates on top of the pre-grouping guys and always uses 1/2 overlap spacing

  int		pre_rf;		// #READ_ONLY size of pre-grouping -- always 4 for now, as it is the only thing that makes sense
  int		pre_half;	// #READ_ONLY pre_rf / 2
  int		pre_spacing;	// #READ_ONLY pre_half always
  int		pre_border;	// #READ_ONLY border onto v1s filters -- automatically computed based on wrap mode and spacing setting

  TwoDCoord	spat_half;	// #READ_ONLY half rf
  TwoDCoord	spat_spacing;	// #READ_ONLY 1/2 overlap spacing with spat_rf
  TwoDCoord	spat_border;	// #READ_ONLY border onto v1s filters -- automatically computed based on wrap mode and spacing setting

  TwoDCoord	net_rf;		// #READ_ONLY net = pre * spat
  TwoDCoord	net_half;	// #READ_ONLY net = pre * spat
  TwoDCoord	net_spacing;	// #READ_ONLY net = pre * spat
  TwoDCoord	net_border;	// #READ_ONLY net = pre * spat

  int		len_sum_width;	// #READ_ONLY 1 + 2 * len_sum_len -- computed
  float		len_sum_norm;	// #READ_ONLY 1.0 / len_sum_width -- normalize sum

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V1ComplexSpec);
protected:
  void 	UpdateAfterEdit_impl();
};


class TA_API V1RegionSpec : public VisRegionSpecBase {
  // #STEM_BASE ##CAT_Image specifies a region of V1 simple and complex filters -- used as part of overall V1Proc processing object -- takes retinal DoG filter inputs and produces filter activation outputs -- each region is a separate matrix column in a data table (and network layer), and has a specified spatial resolution
INHERITED(VisRegionSpecBase)
public:
  enum ComplexFilters { // #BITS flags for specifying which complex filters to include
    CF_NONE	= 0, // #NO_BIT
    END_STOP	= 0x0001, // end stop cells -- opposite polarity, any orientation around a central oriented edge, max over all polarities (weighted by gaussian) 
    LEN_SUM	= 0x0002, // length summing cells -- integrate simple cells along a line, same polarity, max over all polarities (weighted by gaussian) 
    V1S_MAX    	= 0x0004, // basic max over v1 simple cells (weighted by gaussian) -- preserves polarity -- only for on/off (monochrome) tuning in v1s -- provides useful lower-level, higher-res signals to higher levels and should be on by default
    BLOB	= 0x0008, // 'blobs' made by integrating over all angles for a given luminance or color contrast -- adds 2 units for white/black per hypercolumn (always avail), and if color = COLOR, adds 6 extra units for red/cyan on/off, green/magenta on/off, blue/yellow on/off
    DISP_EDGE	= 0x0010, // respond to an edge in disparity, integrating over all other simple cell tunings (orientation, polarity etc) -- only applicable if BINOCULAR ocularity
    MOTION_EDGE	= 0x0020, // respond to an edge in motion, integrating over all other simple cell tunings (orientation, polarity etc) -- only applicable if motion_frames > 1
#ifndef __MAKETA__
    CF_ESLS	= END_STOP | LEN_SUM, // #IGNORE #NO_BIT most basic set
    CF_ESLSMAX	= END_STOP | LEN_SUM | V1S_MAX, // #IGNORE #NO_BIT this is the default setup
    CF_DEFAULT  = CF_ESLSMAX,  // #IGNORE #NO_BIT this is the default setup
    CF_COLOR	= CF_DEFAULT | BLOB, // #IGNORE #NO_BIT default + blob = color setup
    CF_EDGES	= DISP_EDGE | MOTION_EDGE, // #IGNORE #NO_BIT special complex edges
#endif
  };

  enum OptionalFilters { // #BITS flags for specifying additional output filters -- misc grab bag of outputs
    OF_NONE	= 0, // #NO_BIT
    ENERGY	= 0x0001, // overall energy of V1 feature detectors -- a single unit activation per spatial location -- output map is just 2D, not 4D -- and is always saved to a separate output column -- suitable for basic spatial mapping etc
    V1B_MAX	= 0x0002, // basic max over v1b cells (weighted by gaussian) -- preserves depth coding and polarity -- allows disparity info to be at same resolution as other v1c data -- this always saved to a separate output and can be normed separately
    V1B_DSP	= 0x0004, // disparity information only extracted from underlying disparity map -- has no orientation signals -- has same geometry as V1S output
    V1B_AVGSUM	= 0x0008, // compute weighted average summary of the disparity signals over entire field -- result is a single scalar value that can be fed into a ScalarValLayerSpec layer to provide an input representation to a network, for example -- this value is always just computed separately as a single 1x1 matrix cell
  };


  enum V1Filters { // different stages of V1 filters
    V1_SIMPLE,	   // V1 Simple oriented, polarity sensitive cells
    V1_BINOCULAR,  // V1 Binocular integration 
    V1_COMPLEX,	   // V1 Complex cells
  };

  enum XY {	   // x, y component of stencils etc -- for clarity in code
    X,
    Y,
  };
  enum LnOrtho {   // line, orthogonal to the line -- for v1s_ang_slopes
    LINE,	   // along the direction of the line
    ORTHO,	   // orthogonal to the line
  };
  enum DspMatch {	// for storing disparity match information
    DSP_OFF,		// disparity offset
    DSP_DIST,		// feature activation absolute distance at given offset
    DSP_CNT,		// count of number of matching feature locations supporting this disparity estimate
    DSP_N,		// number of disparity match values to record
  };

  enum DspFlags {	// flags for disparity computation -- mutex so not bitflags
    DSP_NONE,		// no flag
    DSP_AMBIG_N,	// indicates that the disparity info at this location is ambiguous due to an excessive number of matches -- likely due to the apeture problem for horizontal features -- can try to fill in based on other constraints
    DSP_AMBIG_THR,	// indicates that the disparity info at this location is ambiguous due to the threshold being exceeded -- no good bottom-up matches were available -- can try to fill in based on other constraints
  };


  V1GaborSpec	v1s_specs;	// specs for V1 simple filters, computed using gabor filters directly onto the incoming image
  V1KwtaSpec	v1s_kwta;	// k-winner-take-all inhibitory dynamics for the v1 simple stage -- important for cleaning up these representations for subsequent stages, especially binocluar disparity and motion processing, which require correspondence matching, and end stop detection
  V1sNeighInhib	v1s_neigh_inhib; // specs for V1 simple neighborhood-feature inhibition -- inhibition spreads in directions orthogonal to the orientation of the features, to prevent ghosting effects around edges
  V1MotionSpec	v1s_motion;	// #CONDSHOW_OFF_motion_frames:0||1 specs for V1 motion filters within the simple processing layer
  RenormMode	v1s_renorm;	// #DEF_LIN_RENORM how to renormalize the output of v1s static filters
  RenormMode	v1m_renorm;	// #CONDSHOW_OFF_motion_frames:0||1 #DEF_NO_RENORM how to renormalize the output of v1s motion filters
  DataSave	v1s_save;	// how to save the V1 simple outputs for the current time step in the data table
  XYNGeom	v1s_feat_geom; 	// #READ_ONLY #SHOW size of one 'hypercolumn' of features for V1 simple filtering -- n_angles (x) * 2 or 6 polarities (y; monochrome|color) + motion: n_angles (x) * 2 polarities (y=0, y=1) * 2 directions (next level of y) * n_speeds (outer y dim) -- configured automatically
  XYNGeom	v1s_img_geom; 	// #READ_ONLY #SHOW size of v1 simple filtered image output -- number of hypercolumns in each axis to cover entire output -- this is equivalent to dog_img_geom

  int		v1s_feat_mot_y;	// #READ_ONLY y axis index for start of motion features in v1s -- x axis is angles

  V1BinocularSpec v1b_specs;	// #CONDSHOW_ON_region.ocularity:BINOCULAR specs for V1 binocular filters -- comes after V1 simple processing in binocular case
  V1DispSpreadSpec v1b_dsp_specs; // #CONDSHOW_ON_region.ocularity:BINOCULAR specs for disparity spreading in the V1 binocular filters
  DataSave	v1b_save;	// #CONDSHOW_ON_region.ocularity:BINOCULAR how to save the V1 binocular outputs for the current time step in the data table
  XYNGeom	v1b_feat_geom; 	// #CONDSHOW_ON_region.ocularity:BINOCULAR #READ_ONLY #SHOW size of one 'hypercolumn' of features for V1 binocular -- (1 + 2*n_disps) * v1s_feat_geom -- order: near, focus, far
  XYNGeom	v1b_img_geom; 	// #CONDSHOW_ON_region.ocularity:BINOCULAR #READ_ONLY #HIDDEN size of v1 binocular filtered image output -- number of hypercolumns in each axis to cover entire output -- this is copied directly from v1s_img_geom

  ComplexFilters v1c_filters; 	// which complex cell filtering to perform
  V1ComplexSpec v1c_specs;	// specs for V1 complex filters -- comes after V1 binocular processing 
  V1KwtaSpec	v1c_kwta;	// k-winner-take-all inhibitory dynamics for the v1 complex stage -- in general only use this when NOT otherwise using leabra, because these inputs will go into leabra anyway
  RenormMode	v1c_renorm;	// #DEF_LIN_RENORM how to renormalize the output of v1c filters
  ComplexFilters v1c_sep_renorm; // any filters here will be renormalized separately prior to computing the kwta -- this puts them on more of an equal footing if there are underlying imbalances -- only relevant ones at this point are END_STOP, LEN_SUM, and V1S_MAX
  RenormMode	v1bmax_renorm;	// #CONDSHOW_ON_v1c_filters:V1B_MAX #DEF_LIN_RENORM how to renormalize the output of v1b_max filters
  DataSave	v1c_save;	// how to save the V1 complex outputs for the current time step in the data table
  XYNGeom	v1c_feat_geom; 	// #READ_ONLY #SHOW size of one 'hypercolumn' of features for V1 complex filtering -- configured automatically with x = n_angles
  XYNGeom	v1c_pre_geom; 	// #READ_ONLY size of v1 complex pre-grouping of v1s or v1b values -- number of hypercolumns in each axis to cover entire output -- this is determined by v1c pre_rf on top of v1s img geom -- feature size is equivalent to v1s_feat_geom
  XYNGeom	v1c_img_geom; 	// #READ_ONLY #SHOW size of v1 complex filtered image output -- number of hypercolumns in each axis to cover entire output -- this is determined by v1c rf sz on top of v1s img geom

  OptionalFilters opt_filters; 	// optional filter outputs -- always rendered to separate tables in data table
  DataSave	opt_save;	// how to save the optional outputs for the current time step in the data table

  ///////////////////  V1S Geom/Stencils ////////////////////////
  int		n_colors;	// #READ_ONLY number of color channels to be processed (1 = monochrome, 4 = full color)
  int		n_polarities;	// #READ_ONLY #DEF_2 number of polarities per color -- always 2
  int		n_polclr;	// #READ_ONLY number of polarities * number of colors -- y dimension of simple features for example
  float_Matrix	v1s_gabor_filters; // #READ_ONLY #NO_SAVE gabor filters for v1s processing [filter_size][filter_size][n_angles]
  float_Matrix	v1s_ang_slopes; // #READ_ONLY #NO_SAVE angle slopes [dx,dy][line,ortho][angles] -- dx, dy slopes for lines and orthogonal lines for each fo the angles
  int_Matrix	v1s_ni_stencils; // #READ_ONLY #NO_SAVE stencils for neighborhood inhibition [x,y][tot_ni_len][angles]
  float_Matrix	v1m_weights;  	// #READ_ONLY #NO_SAVE v1 simple motion weighting factors (1d)
  int_Matrix	v1m_stencils; 	// #READ_ONLY #NO_SAVE stencils for motion detectors, in terms of v1s location offsets through time [x,y][1+2*tuning_width][motion_frames][directions:2][angles][speeds] (6d)

  ///////////////////  V1B Geom/Stencils ////////////////////////
  int_Matrix	v1b_widths; 	// #READ_ONLY #NO_SAVE width of stencils for binocularity detectors 1d: [tot_disps]
  float_Matrix	v1b_weights;	// #READ_ONLY #NO_SAVE v1 binocular weighting factors -- for each tuning disparity [max_width][tot_disps] -- only v1b_widths[disp] are used per disparity
  int_Matrix	v1b_stencils; 	// #READ_ONLY #NO_SAVE stencils for binocularity detectors, in terms of v1s location offsets per image: 2d: [XY][max_width][tot_disps]
  int_Matrix	v1b_dsp_stencils; // #READ_ONLY #NO_SAVE stencils for binocularity spreading dynamics -- neighborhood to integrate over: [XY][tot_width][n_angles]
  float_Matrix	v1b_dsp_weights; // #READ_ONLY #NO_SAVE weighting factors for neighborhood spreading of binocularity [tot_width]
  float_Matrix	v1bc_weights;	// #READ_ONLY #NO_SAVE weighting factors for integration from binocular disparities to complex responses
  // v1c feat x axis is always angle, except for final edge guys and blobs

  ///////////////////  V1C Geom/Stencils ////////////////////////
  int		v1c_feat_es_y;	// #READ_ONLY y axis index for start of end stop features in v1c
  int		v1c_feat_ls_y;	// #READ_ONLY y axis index for start of length sum features in v1c
  int		v1c_feat_smax_y; // #READ_ONLY y axis index for start of v1s_max features in v1c
  int		v1c_feat_blob_y; // #READ_ONLY y axis index for start of blob features in v1c
  int		v1c_feat_edge_y; // #READ_ONLY y axis index for start of edge features in v1c (disp, motion)
  float_Matrix	v1c_weights;	// #READ_ONLY #NO_SAVE v1 complex spatial weighting factors (2d)
  int_Matrix	v1c_gp4_stencils; // #READ_ONLY #NO_SAVE stencils for v1c pre_gp4 pre-grouping -- represents center points of the lines for each angle [x,y,len][5][angles] -- there are 5 points for the 2 diagonal lines with 4 angles -- only works if n_angles = 4 and line_len = 4 or 5
  int_Matrix	v1c_es_stencils;  // #READ_ONLY #NO_SAVE stencils for complex end stop cells [x,y][sum_line=2][max_line=2][angles]
  int_Matrix	v1c_ls_stencils;  // #READ_ONLY #NO_SAVE stencils for complex length sum cells [x,y][len_sum_width][angles]
  float_Matrix	v1c_es_angwts;  // #READ_ONLY #NO_SAVE weights for different angles relative to a given angle [n_angles][n_angles]

  ///////////////////  V1S Output ////////////////////////
  float_Matrix	v1s_out_l_raw;	 // #READ_ONLY #NO_SAVE raw (pre kwta) v1 simple cell output, left eye [feat.x][feat.y][img.x][img.y][time] -- time is optional depending on motion_frames -- feat.y = [0=on,1=off,2-6=colors if used,motion:n=on,+dir,speed1,n+1=off,+dir,speed1,n+2=on,-dir,speed1,n+3=off,-dir,speed1, etc.
  float_Matrix	v1s_out_r_raw;	 // #READ_ONLY #NO_SAVE raw (pre kwta) v1 simple cell output, right eye [feat.x][feat.y][img.x][img.y][time] -- time is optional depending on motion_frames -- feat.y = [0=on,1=off,2-6=colors if used,motion:n=on,+dir,speed1,n+1=off,+dir,speed1,n+2=on,-dir,speed1,n+3=off,-dir,speed1, etc.
  float_Matrix	v1s_gci;	 // #READ_ONLY #NO_SAVE v1 simple cell inhibitory conductances, for computing kwta
  float_Matrix	v1s_ithr;	 // #READ_ONLY #NO_SAVE v1 simple cell inhibitory threshold values -- intermediate vals used in computing kwta
  float_Matrix	v1s_out_l;	 // #READ_ONLY #NO_SAVE v1 simple cell output, left eye [feat.x][feat.y][img.x][img.y][time] -- time is optional depending on motion_frames -- feat.y = [0=on,1=off,2-6=colors if used,motion:n=on,+dir,speed1,n+1=off,+dir,speed1,n+2=on,-dir,speed1,n+3=off,-dir,speed1, etc.
  float_Matrix	v1s_out_r;	 // #READ_ONLY #NO_SAVE v1 simple cell output, right eye [feat.x][feat.y][img.x][img.y][time] -- time is optional depending on motion_frames -- feat.y = [0=on,1=off,2-6=colors if used,motion:n=on,+dir,speed1,n+1=off,+dir,speed1,n+2=on,-dir,speed1,n+3=off,-dir,speed1, etc.
  CircMatrix	v1s_circ_r;  	 // #NO_SAVE #NO_COPY #READ_ONLY circular buffer indexing for time
  CircMatrix	v1s_circ_l;  	 // #NO_SAVE #NO_COPY #READ_ONLY circular buffer indexing for time

  ///////////////////  V1B Output ////////////////////////
  int_Matrix	v1b_dsp_nmatch;	 // #READ_ONLY #NO_SAVE number of top matches -- tells how many actual matches are in v1b_dsp_match [img.x][img.y]
  int_Matrix	v1b_dsp_flags;	 // #READ_ONLY #NO_SAVE flags to indicate status of match for this location [img.x][img.y]
  float_Matrix	v1b_dsp_match;	 // #READ_ONLY #NO_SAVE list of top matches -- aggregates across features (average abs distance across features) -- values are offset and min dist at that index for each match, or DSP_AMBIG_HORIZ (see DspMatch enum) [2][n_matches][img.x][img.y]
  float_Matrix	v1b_dsp_win;	 // #READ_ONLY #NO_SAVE results of the window aggregation process for each point -- integrates over spatial window and features -- result is the best match offset, min dist at that offset, and count of feature matches at that offset, or ambiguous [DSP_N][img.x][img.y]
  float_Matrix	v1b_dsp_out;	 // #READ_ONLY #NO_SAVE pre-grouping as basis for subsequent v1c filtering on the v1b reps -- reduces dimensionality and introduces robustness [v1b_feat.x][v1b_feat.y][v1c_pre.x][v1c_pre.y]
  float_Matrix	v1b_out;	 // #READ_ONLY #NO_SAVE v1 binocular output, which is v1s feature activation times v1b_dsp_win weighting per feature -- [feat.x][feat.y][img.x][img.y]

  ///////////////////  V1C Output ////////////////////////
  float_Matrix	v1c_v1b_pre;	 // #READ_ONLY #NO_SAVE reduce v1b features back down to v1s size by collapsing across disparity, but with focal region differentially weighted [v1s_feat.x][v1s_feat.y][v1s_img.x][v1s_img.y]
  float_Matrix	v1c_pre;	 // #READ_ONLY #NO_SAVE pre-grouping as basis for subsequent v1c filtering -- reduces dimensionality and introduces robustness [v1s_feat.x][v1s_feat.y][v1c_pre.x][v1c_pre.y]
  float_Matrix	v1c_pre_polinv;	 // #READ_ONLY #NO_SAVE polarity invariant version of v1c_pre -- used for end stop filters.. [v1s_feat.x][1a][v1c_pre.x][v1c_pre.y]
  float_Matrix	v1c_esls_raw;	 // #READ_ONLY #NO_SAVE raw (pre spatial integration, but post v1c_pre) v1 complex end-stop and length-sum output [feat.x][2][v1s_pre.x][v1s_pre.y]
  float_Matrix	v1c_out_raw;	 // #READ_ONLY #NO_SAVE raw (pre kwta) v1 complex output [feat.x][feat.y][img.x][img.y]
  float_Matrix	v1c_gci;	 // #READ_ONLY #NO_SAVE v1 complex cell inhibitory conductances, for computing kwta
  float_Matrix	v1c_out;	 // #READ_ONLY #NO_SAVE v1 complex output [v1c_feat.x][v1c_feat.y][v1c_img.x][v1c_img.y]

  ///////////////////  OPT optional Output ////////////////////////
  float_Matrix	v1bmax_pre;	 // #READ_ONLY #NO_SAVE pre-grouping as basis for subsequent v1c filtering on the v1b reps -- reduces dimensionality and introduces robustness [v1b_feat.x][v1b_feat.y][v1c_pre.x][v1c_pre.y]
  float_Matrix	v1bmax_out;	 // #READ_ONLY #NO_SAVE v1b max output [v1b_feat.x][v1b_feat.y][v1c_img.x][v1c_img.y]
  float_Matrix	energy_out;	 // #READ_ONLY #NO_SAVE v1 complex energy output [v1c_img.x][v1c_img.y]
  float		v1b_avgsum_out;	 // #READ_ONLY #NO_SAVE v1b avgsum output (single scalar value)

  int		AngleDeg(int ang_no);
  // get angle value in degress based on angle number
  virtual void	GridGaborFilters(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot all of the V1 Gabor Filters into the data table
  virtual void	GridV1Stencils(DataTable* disp_data);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable plot all of the V1 stencils into data table and generate a grid view -- these are the effective receptive fields at each level of processing
  virtual void	PlotSpacing(DataTable* disp_data, bool reset = true);
  // #BUTTON #NULL_OK_0 #NULL_TEXT_0_NewDataTable #ARGC_1 plot the arrangement of the filters (centers) in the data table using given value, and generate a grid view -- one row for each type of filter (scroll to see each in turn) -- light squares show bounding box of rf, skipping every other

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(V1RegionSpec);
protected:
  float_Matrix* cur_out_acts;	// cur output activations -- for kwta thing

  override void	UpdateAfterEdit_impl();

  override void	UpdateGeom();

  override bool NeedsInit();
  override bool InitFilters();
  override bool InitOutMatrix();
  override bool InitDataTable();

  virtual bool	InitFilters_V1Simple();
  virtual bool	InitFilters_V1Motion();
  virtual bool	InitFilters_V1Binocular();
  virtual bool	InitFilters_V1Complex();

  override bool	FilterImage_impl();
  override void IncrTime();

  virtual bool	V1SimpleFilter_Static(float_Matrix* image, float_Matrix* out_raw,
				      float_Matrix* out, CircMatrix* circ);
  // do simple filters, static only on current inputs -- dispatch threads
  virtual void 	V1SimpleFilter_Static_thread(int v1s_idx, int thread_no);
  // do simple filters, static only on current inputs -- do it
  virtual void 	V1SimpleFilter_Static_neighinhib_thread(int v1s_idx, int thread_no);
  // do neighborhood inhibition on simple filters

  virtual bool	V1SimpleFilter_Motion(float_Matrix* out, CircMatrix* circ);
  // do simple filters, static only on current inputs -- dispatch threads
  virtual void 	V1SimpleFilter_Motion_thread(int v1s_idx, int thread_no);
  // do simple filters, static only on current inputs -- do it

  virtual bool	V1SRenormOutput_Static(float_Matrix* out, CircMatrix* circ);
  virtual bool	V1SRenormOutput_Motion(float_Matrix* out, CircMatrix* circ);

  virtual bool	V1BinocularFilter();
  // do binocular filters -- dispatch threads
  virtual void 	V1BinocularFilter_Match_thread(int v1b_idx, int thread_no);
  // do binocular filters -- compute initial matches between eyes
  virtual void 	V1BinocularFilter_WinAgg_thread(int v1b_idx, int thread_no);
  // do binocular filters -- aggregate over window and identify best match based on larger context
  virtual void 	V1BinocularFilter_Out_thread(int v1b_idx, int thread_no);
  // do binocular filters -- compute final output from disparity weightings

  virtual bool	V1ComplexFilter();
  // do complex filters -- dispatch threads
  virtual void 	V1ComplexFilter_Pre_Monocular_thread(int v1c_pre_idx, int thread_no);
  // monocular inputs -- pre-grouping for subsequent processing
  virtual void 	V1ComplexFilter_BinocularInteg_thread(int v1b_idx, int thread_no);
  // binocular inputs -- integration (collapsing) across binocular inputs
  virtual void 	V1ComplexFilter_Pre_Binocular_thread(int v1c_pre_idx, int thread_no);
  // binocular inputs -- pre-grouping for subsequent processing
  virtual void 	V1ComplexFilter_Pre_Polinv_thread(int v1c_pre_idx, int thread_no);
  // polarity invariance for pre pass
  virtual void 	V1ComplexFilter_EsLs_Raw_thread(int v1c_pre_idx, int thread_no);
  // pre-grouped inputs -- EndStop & Length Sum
  virtual void 	V1ComplexFilter_EsLs_Integ_thread(int v1c_idx, int thread_no);
  // pre-grouped inputs -- EndStop & Length Sum
  virtual void 	V1ComplexFilter_V1SMax_thread(int v1c_idx, int thread_no);
  // pre-grouped inputs -- V1Simple Max
  virtual void 	V1ComplexFilter_Blob_thread(int v1c_idx, int thread_no);
  // pre-grouped inputs -- Blob
  virtual void 	V1ComplexFilter_DispEdge_thread(int v1c_idx, int thread_no);
  // binocular inputs -- disparity edge
  virtual void 	V1ComplexFilter_MotionEdge_thread(int v1c_idx, int thread_no);
  // pre-grouped inputs -- motion edge

  virtual bool	V1OptionalFilter();
  // do optional filters -- dispatch threads

  virtual void 	V1ComplexFilter_Pre_V1BMax_thread(int v1c_pre_idx, int thread_no);
  // v1b -> v1b_pre -- do the 4x4 pregroup for v1bmax
  virtual void 	V1ComplexFilter_V1BMax_thread(int v1c_idx, int thread_no);
  // pre-grouped inputs -- V1Binocular Max
  virtual void 	V1ComplexFilter_Energy_thread(int v1c_idx, int thread_no);
  // pre-grouped inputs -- Energy
  virtual void 	V1ComplexFilter_V1BAvgSum();
  // v1 binocular weighted-average summary

  virtual bool	V1CRenormOutput_EsLsBlob(float_Matrix* out);
  // end stop, length sum, blob separate renorm

  virtual bool V1SOutputToTable(DataTable* dtab);
  // simple to output table
  virtual bool V1SOutputToTable_impl(DataTable* dtab, float_Matrix* out, CircMatrix* circ,
				     const String& col_sufx);
  // simple to output table impl
  virtual bool V1BOutputToTable(DataTable* dtab);
  // binocular to output table
  virtual bool V1COutputToTable(DataTable* dtab);
  // complex to output table
  virtual bool OptOutputToTable(DataTable* dtab);
  // optional to output table

  override bool	InvertFilters_impl();
  virtual bool	V1ComplexInvertFilter();
  // invert complex filters
  virtual bool	V1ComplexInvertFilter_EsLs_Monocular();
  // invert complex filters - EndStop & Length Sum
  virtual bool	V1ComplexInvertFilter_V1SMax_Monocular();
  // invert complex filters - V1Simple Max
  virtual bool	V1SimpleInvertFilter_Static(float_Matrix* dog, CircMatrix* dog_circ,
					    float_Matrix* out_raw, float_Matrix* out, 
					    CircMatrix* circ);
  // invert simple filters, static only on current inputs

};


class TA_API RetinaProc : public taNBase {
  // #STEM_BASE ##CAT_Image ##DEF_CHILD_regions ##DEF_CHILDNAME_Regions full specification of retinal filtering -- takes raw input images, applies various transforms, and then runs through filtering -- first region is used for retina size and other basic params
INHERITED(taNBase)
public:
  taImageProc::EdgeMode	edge_mode;	// how to deal with edges in processing the raw images in preparation for presentation to the filters -- each region has its own filter-specific edge mode which is not automatically sync'd with this one (and they can be different)
  int 			fade_width;	// #CONDSHOW_ON_edge_mode:BORDER for border mode -- how wide of a frame to fade in around the border at the end of all the operations 
  VisRegionSpecBaseList	regions;	// defines regions of the visual input where the processing actually takes place -- most of the specification is at this level -- first region is used for retina size and other basic params

  float_Matrix		raw_image_r; 	// #READ_ONLY #NO_SAVE current raw input image presented to system, for right eye or only eye if monocular
  float_Matrix		raw_image_l; 	// #READ_ONLY #NO_SAVE current raw input image presented to system, for left eye -- only if binocular

  float_Matrix		xform_image_r; 	// #READ_ONLY #NO_SAVE current transformed version of raw image presented to system, for right eye or only eye if monocular
  float_Matrix		xform_image_l; 	// #READ_ONLY #NO_SAVE current transformed version of raw image presented to system, for left eye -- only if binocular

  virtual VisRegionSpecBase* AddRegion()	{ return (VisRegionSpecBase*)regions.New(1); }
  // #BUTTON #CAT_Filter add a new region -- type is whatever the default is for this type of retina processor

  ///////////////////////////////////////////////////////////////////////
  // Basic functions operating on float image data: transform image, apply dog filters

  virtual bool	Init();
  // #BUTTON initialize the filters, data table, etc -- call this in the init_code of any Program that is using this object

  virtual bool	TransformImageData(float_Matrix* right_eye_image,
				   float_Matrix* left_eye_image = NULL, 
				   float move_x=0.0f, float move_y=0.0f,
				   float scale = 1.0f, float rotate = 0.0f);
  // #CAT_Transform transform image data in matrix format, with retina centered at given normalized offsets from center of image (move -1=far left edge, 0=ctr, 1=far right edge), scaled by given factor (1=same size, .5 = half size, 2 = double size), rotated by normalized units (1=360deg)

  virtual bool	LookAtImageData(float_Matrix* right_eye_image,
				float_Matrix* left_eye_image = NULL, 
				VisRegionParams::Region region = VisRegionParams::FOVEA,
				float box_ll_x=0.0f, float box_ll_y=0.0f,
				float box_ur_x=1.0f, float box_ur_y=1.0f,
				float move_x=0.0f, float move_y=0.0f,
				float scale = 1.0f, float rotate = 0.0f);
  // #CAT_Transform transform image data in matrix format, with region of retina centered and scaled to fit the box coordinates given in 0-1 normalized units (ll=lower-left, ur=upper-right); additional scale, rotate, and move params applied after foveation scaling and offsets

  virtual bool	FilterImageData();
  // #CAT_Filter filter retinal image data -- operates on images that were generated from prior Transform or LookAt calls -- must call one of those first.


  ///////////////////////////////////////////////////////////////////////
  // Transform Routines taking different sources for image input data

  virtual bool  ConvertImageToMatrix(float_Matrix& img_data, taImage* img, 
				     VisRegionParams::Color color);
  // #CAT_Image convert image file to img_data float matrix, converting to color or monochrome as specified

  virtual bool	TransformImage(taImage* right_eye_image, taImage* left_eye_image = NULL,
			       float move_x=0, float move_y=0,
			       float scale = 1.0f, float rotate = 0.0f);
  // #CAT_Transform transform image/s in image format, with retina centered at given normalized offsets from center of image (move -1=far left edge, 0=ctr, 1=far right edge), scaled by given factor (1=same size, .5 = half size, 2 = double size), rotated by normalized units (1=360deg)

  virtual bool	TransformImageName(const String& right_eye_img_fname,
				   const String& left_eye_img_fname = "",
				   float move_x=0, float move_y=0,
				   float scale = 1.0f, float rotate = 0.0f);
  // #BUTTON #CAT_Transform #FILE_DIALOG_LOAD load image/s from file/s and transform image data, with retina centered at given normalized offsets from center of image (move: -1=far left edge, 0=ctr, 1=far right edge), scaled by given factor (zoom), rotated by normalized units (1=360deg)

  virtual bool	LookAtImage(taImage* right_eye_image,
			    taImage* left_eye_image = NULL, 
			    VisRegionParams::Region region = VisRegionParams::FOVEA,
			    float box_ll_x=0.0f, float box_ll_y=0.0f,
			    float box_ur_x=1.0f, float box_ur_y=1.0f,
			    float move_x=0.0f, float move_y=0.0f,
			    float scale = 1.0f, float rotate = 0.0f);
  // #CAT_Transform transform image/s in image format, with region of retina centered and scaled to fit the box coordinates given in 0-1 normalized units (ll=lower-left, ur=upper-right); additional scale, rotate, and move params applied after foveation scaling and offsets

  virtual bool	LookAtImageName(const String& right_eye_img_fname,
				const String& left_eye_img_fname = "",
				VisRegionParams::Region region = VisRegionParams::FOVEA,
				float box_ll_x=0.0f, float box_ll_y=0.0f,
				float box_ur_x=1.0f, float box_ur_y=1.0f,
				float move_x=0, float move_y=0,
				float scale = 1.0f, float rotate = 0.0f);
  // #BUTTON #CAT_Filter #FILE_DIALOG_LOAD load image/s from file/s and transform image data, with region of retina centered and scaled to fit the box coordinates given (ll=lower-left coordinates, in pct; ur=upper-right); additional scale, rotate, and offset params add to foveation scaling and offsets

  ///////////////////////////////////////////////////////////////////////
  // Misc other processing operations

  virtual bool	InvertFilter();
  // #CAT_Filter #BUTTON invert the filter -- uses current contents of the v1c_out complex filter values within each region to re-generate an image via all the intermediate transforms along the way -- due to extensive information loss at each step of the transformation, the resulting image will be significantly different from a corresponding input, but hopefully at least somewhat recognizable -- results written to image columns of data table

  virtual bool	AttendRegion(DataTable* dt, VisRegionParams::Region region = VisRegionParams::FOVEA);
  // #CAT_Filter apply attentional weighting filter to filtered values, with radius = given region

  // todo: need a checkconfig here..

  override taList_impl*	children_() {return &regions;}	
  override void*	GetTA_Element(Variant i, TypeDef*& eltd)
  { return regions.GetTA_Element(i, eltd); }

  void 	Initialize();
  void	Destroy() { };
  TA_SIMPLE_BASEFUNS(RetinaProc);
protected:
  void	UpdateAfterEdit_impl();
  override void CheckChildConfig_impl(bool quiet, bool& rval);

  virtual bool	TransformImageData_impl(float_Matrix& eye_image,
					float_Matrix& xform_image,
					float move_x=0.0f, float move_y=0.0f,
					float scale = 1.0f, float rotate = 0.0f);
  // implementation function that processes an eye input image into an xform output image

  virtual bool	LookAtImageData_impl(float_Matrix& eye_image,
				     float_Matrix& xform_image,
				     VisRegionParams::Region region = VisRegionParams::FOVEA,
				     float box_ll_x=0.0f, float box_ll_y=0.0f,
				     float box_ur_x=1.0f, float box_ur_y=1.0f,
				     float move_x=0.0f, float move_y=0.0f,
				     float scale = 1.0f, float rotate = 0.0f);
  // implementation function that processes an eye input image into an xform output image

};

SmartRef_Of(RetinaProc,TA_RetinaProc); // RetinaProcRef

class TA_API DoGRetinaProc : public RetinaProc {
  // Difference-of-Gaussians version of retinal filtering -- takes raw input images, applies various transforms, and then runs through filtering -- first region is used for retina size and other basic params
INHERITED(RetinaProc)
public:
  virtual VisRegionSpecBase* AddRegion()
  { return (VisRegionSpecBase*)regions.New(1, &TA_DoGRegionSpec); }

  void 	Initialize();
  void	Destroy() { };
  TA_BASEFUNS_NOCOPY(DoGRetinaProc);
};

class TA_API V1RetinaProc : public RetinaProc {
  // V1 version of retinal filtering -- takes raw input images, applies various transforms, and then runs through filtering -- first region is used for retina size and other basic params
INHERITED(RetinaProc)
public:
  virtual VisRegionSpecBase* AddRegion()
  { return (VisRegionSpecBase*)regions.New(1, &TA_V1RegionSpec); }

  void 	Initialize();
  void	Destroy() { };
  TA_BASEFUNS_NOCOPY(V1RetinaProc);
};

/////////////////////////////////////////////////////////
//   programs to support image processing operations
/////////////////////////////////////////////////////////

class TA_API ImageProcCall : public StaticMethodCall { 
  // call a taImageProc function
INHERITED(StaticMethodCall)
public:
  override String	GetToolbarName() const { return "img proc()"; }
  TA_BASEFUNS_NOCOPY(ImageProcCall);
private:
  void	Initialize();
  void	Destroy()	{ };
}; 

#endif // ta_imgproc_h
