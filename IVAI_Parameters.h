/*
 * IVAI_Parameters.h
 *
 * Created on: 2010/10/13
 * Author Jeff Yang    
 */
#ifndef IVAI_PARAMETERS_H_
#define IVAI_PARAMETERS_H_

#define MAX_OBJECT_AMOUNT	128
#define MAX_POLYGON_AMOUNT	8
#define MAX_POLYGON_POINT	10
#define MAX_PC_GROUP		(MAX_POLYGON_AMOUNT>>1)
#define MAX_WIDTH		1920
#define MAX_HEIGHT		1080

//Modify,Jeff Yang,2010/11/16
#define MAX_RECTANGLE_AMOUNT  (MAX_OBJECT_AMOUNT<<1) //Just for CCL,NIZ!!!

//Modify,Jeff Yang,2010/11/12
#define BACKGROUND   0
#define FOREGROUND   255

//Added,Jeff Yang,2010/12/01  
#define OBJECTS_SIZE_TYPE_DEFINE_AMOUNT	3

//Added,Jeff Yang,2010/12/03  
#define OBJECTS_COLOR_TYPE_DEFINE_AMOUNT 3

//Added, Bruce Hsu, 12/14/2010
#define MAX_OBJECT_COLOR_TYPE	OBJECTSCOLORTYPE_THEBOTTOM+1

#ifdef BGS_CODEBOOK
typedef struct _Oblong_Area
{
	unsigned int nLeftTopX;
	unsigned int nLeftTopY;
	unsigned int nRightBottomX;
	unsigned int nRightBottomY;

} Oblong_Area;

typedef struct BGS_Parameters
{
	float ALPHA;
	float BETA;
	unsigned int TRAINING_FRAME;// 50
	unsigned int UPDATED_FRAME;   //20
	//int define RANGE 30  //Used in BG_different. Higher RANGE, the CodeWord fit range is bigger -> reduce the noise
	unsigned char CB_BOUNDS; //20
	unsigned char UPDATE_THRESHOLD; //20
	int SMOOTHLY_REFER_AREA_THRESHOLD; //default: 3~5
	int LIGHT_CHANGE_THRESHOLD; //user define , case by case
	Oblong_Area smoothlychange_detect_area;
	//Default 5 areas : left-up, right-up, left-down, right-down, up-midden
	Oblong_Area suddenly_change_detect_area[5];

} BGS_Parameters;
#else
typedef struct BGS_Parameters
{
	/* �d�� 0~255 */
	/* �P�I���t�Ȧh�ֽd�򤺵����e�� EX: set 5 ��ܫܤ֪���|�Q�����e��, set 125 ��ܫܦh����|�Q�����e��(�]�A���T...) */
	/* Default 20 */
	int LOW_THRESHOLD; 
	/* �d�� �ۭq (�i�̾�fps) */
	/* �e�������s�����I���t�v EX: set a lower value objects will become background faster */
	/* Default 30 */
	int SAMPLING_RATE_CB;
	/* �d�� �ۭq (�i�̾�fps) */
	/* �Ұʵ{���� �ݭnLEARNING_FRAMES�i frames�ӫإ� background model  */
	/* Default 60 */
	unsigned int LEARNING_FRAMES;

} BGS_Parameters;
#endif

typedef struct BTR_Parameters
{
	/* �d�� �ۭq  */
	/* �ثe���]�w�P 1.����j�M�d���� => Search range = MAX2(nWidth,nHeight)/( BLOB_TRACKER_FPS>>1 ) */
	/* 2.ABANDONED_COUNT_THRESHOLD...������ */
	/* Default 30 */
	int BLOB_TRACKER_FPS;
	/* �d�� �̾�BLOB_TRACKER_FPS �ثe���]�w "BLOB_TRACKER_FPS * 10" */
	/* �e������h�֭�frames��|�Q����Abanded �P BGS�e�������s�����I���t�v �����Y EX: set a lower value objects will become ABANDONED objects faster  */
	/* ���~�ثeABANDONED_FUNCTION_DETECTED_TYPE 0 ��release count �ثe�]�w����ABANDONED_COUNT_THRESHOLD	*/
	int ABANDONED_COUNT_THRESHOLD;
	/* �d��	�̾�ABANDONED_COUNT_THRESHOLD  �ثe���]�w "ABANDONED_COUNT_THRESHOLD>>1" */
	/* For ABANDONED_FUNCTION_DETECTED_TYPE 0 �Y���󰱯d�W�L�]�w�� �h����Abanded�Կ� */
	int ABANDONED_SEMI_COUNT_THRESHOLD;
	/* �d�� �̾�ABANDONED_SEMI_COUNT_THRESHOLD�MBLOB_TRACKER_FPS �ثe���]�w "ABANDONED_SEMI_COUNT_THRESHOLD - BLOB_TRACKER_FPS" */
	/* For ABANDONED_FUNCTION_DETECTED_TYPE 0 �Y����l�ܧC�󦹳]�w�� �h�����Ѱ�Abanded�Կ�*/
	int ABANDONED_SEMI_COUNT_RELEASE_THRESHOLD;

} BTR_Parameters;

typedef struct CT_Parameters
{
	/* �d�� �ۭq (�����o��0�M�p��SHORT_TERM_FRAMES)  */
	/* �x�sframes�`�ƥ� ����long term �P�Ԩ��u�ɶ��ܤƥH��warning�ɶ�����  */
	/* Default 240 */
	int STORE_FRAMES;
	/* �d�� �ۭq */
	/* �����x�sframes�`�ƥ� ����short term �P�ܤƥH��warning�ɶ����� */
	/* Default 160 */
	int SHORT_TERM_FRAMES;
	/* �d�� �ۭq */
	/* �W�L���]�w ���� camera tamper �ثe�]�w 320*240=>16000*/
	/* Default 16000 */
	int CAMERA_TAMPER_THRESHOLD;
	/* �d�� �ۭq */
	/* count�W�LCAMERA_TAMPER_THRESHOLD��frames�ƥ� �W�L���]�w�ȫh�o�Xwarning*/
	/* Default 120 */
	int CAMERA_TAMPER_WARNING_COUNT;
	/* �d�� �ۭq */
	/* �w�o�Xwarning������ frames �F�� CAMERA_TAMPER_WARNING_RELEASE_COUNT �h count ����j��CAMERA_TAMPER_WARNINGTURNOFF_COUNT*/
	/* Default 240 */
	int CAMERA_TAMPER_WARNING_RELEASE_COUNT;
	/* �d�� 0 �� 1 */
	/* �]�w��ʩΦ۰ʸѰ�warning EX:set 1 �ߧY�Ѱ�warning */
	/* Default 0 */
	int AUTO_MANUAL_WARNINGTURNOFF;
	/* �d�� CAMERA_TAMPER_WARNING_RELEASE_COUNT<<1 */
	/* count�o�Xwarning��g�L ���]�w��frames�ƥ� release warning*/
	int CAMERA_TAMPER_WARNINGTURNOFF_COUNT;

} CT_Parameters;

typedef struct CCL_Parameters
{
	/* �d�� �ۭq */
	/* �o���p�󦹳]�w ���n������*/
	/* Default 50 */
	int AREA_THRESHOLD;
	//int MAX_RECTANGLE_AMOUNT;

} CCL_Parameters;

typedef struct Color_Parameters
{
	//int Hue;
	int Brightness;
	int Contrast;

} Color_Parameters;

typedef struct JPEG_Parameters
{
	char	Enable;
	//int 	interleave;	//Sec
	char	Path[64];
	

} JPEG_Parameters;

//Added, Bruce Hsu, 11/11/2010
typedef struct point
{
	unsigned int x;
	unsigned int y;

} point;

//Added,Jeff Yang,2010/12/01
//Modified,Jeff Yang,2010/12/06
typedef enum
{
	OBJECTSSIZETYPE_INVALID = -1,		 
	OBJECTSSIZETYPE_TINY,		//0	 
	OBJECTSSIZETYPE_SMALL,		//1 default [0] w 30 h 30
	OBJECTSSIZETYPE_MEDIUM,		//2 default [1] w 60 h 60
	OBJECTSSIZETYPE_LARGE,		//3 default [2] w 90 h 90
	OBJECTSSIZETYPE_HUGE,		//4
	OBJECTSSIZETYPE_THEBOTTOM,	//5 The Bottom

} ObjectsSizeType;

//Added,Jeff Yang,2010/12/01  
//Modified,Jeff Yang,2010/12/03
typedef enum
{
	OBJECTSCOLORTYPE_INVALID = -1,	// 
	OBJECTSCOLORTYPE_RED,			// �� 0
	OBJECTSCOLORTYPE_YELLOW,		// �� 1
	OBJECTSCOLORTYPE_GREEN,			// �� 2
	OBJECTSCOLORTYPE_CYAN,			// �ź�(�C) 3
	OBJECTSCOLORTYPE_BLUE,			// �� 4
	OBJECTSCOLORTYPE_MAGENTA,		// ���� 5
	OBJECTSCOLORTYPE_BLACK,			// �� 6
	OBJECTSCOLORTYPE_WHITE,			// �� 7
	OBJECTSCOLORTYPE_GRAY,			// �� 8
	OBJECTSCOLORTYPE_THEBOTTOM,		// The Bottom 9

} ObjectsColorType;

//Added,Jeff Yang,2010/12/03  
typedef struct objects_color_type_define
{
	ObjectsColorType ColorType;
	unsigned char ColorPercentage; 

} objects_color_type_define;

//Added,Jeff Yang,2010/12/01  
typedef struct objects_size_type_define
{
	unsigned int width;
	unsigned int height;

	ObjectsSizeType OST;

} objects_size_type_define;

typedef struct OTD_Parameters
{
	objects_size_type_define ostd[OBJECTS_SIZE_TYPE_DEFINE_AMOUNT];

} OTD_Parameters;

typedef struct objects_points
{
	unsigned int   nLeftTopX ;
	unsigned int   nLeftTopY ;
	unsigned int   nRightBottomX ;
	unsigned int   nRightBottomY ;
	//Modify,Jeff Yang,2010/11/12        
	int   unObjectID;

	unsigned int   Area;

	//Added,Jeff Yang,2010/12/01
	ObjectsSizeType SizeType;

	//Added,Jeff Yang,2010/12/03
	objects_color_type_define Color[OBJECTS_COLOR_TYPE_DEFINE_AMOUNT];

} objects_points;

typedef struct objects
{
	objects_points ObjectsNumber[MAX_OBJECT_AMOUNT];
	unsigned short usObjectsAmount;

} objects;

//Added,Jeff Yang,2010/12/01
typedef struct dd_group
{
	unsigned char Group;
	unsigned char InOrOut; // In 0 Out 1

} dd_group;

//objects touch DIRECTIONAL DETECTION
typedef struct dd_objects
{
	objects_points DDObjectsNumber[MAX_OBJECT_AMOUNT];
	dd_group DDObjectsGroup[MAX_OBJECT_AMOUNT];
	int DDObjectsAmount;

} dd_objects;

typedef struct TypeInOut
{
        unsigned char   Type;
        unsigned char   nIn;
        unsigned char   nOut;

} TypeInOut;

typedef struct polygon
{
	//point Points[MAX_POLYGON_AMOUNT];
	point Points[MAX_POLYGON_POINT];
	unsigned int nPointsNumber;

} polygon;

typedef enum                                                           
{                                                                      
        FOXINT_ID_INVALID = -1,                                        
        FOXINT_ID_CC,           //Color Converter                      
        FOXINT_ID_COLOR,        //Color Adjustment                     
        FOXINT_ID_BGS,          //Background Subtraction+CC Labeling   
        FOXINT_ID_VF,           //Virtual Fence                        
        FOXINT_ID_LR,           //License Plate Recoginition           
        FOXINT_ID_MO,           //Motion Object Detection              
        FOXINT_ID_SC,           //Scene Change Detection               
        FOXINT_ID_MS,           //Missing Object Detection             
        FOXINT_ID_UO,           //Unattached Object Detection          
        FOXINT_ID_DD0,          //Direction Detection Type 0           
        FOXINT_ID_DD1,          //Direction Detection Type 1           
        FOXINT_ID_PC,           //People Counting                      
        FOXINT_ID_UZ,           //Uninterested Zone                    
        FOXINT_ID_IZ,           //Interested Zone                      
        FOXINT_ID_CD,           //Crowd Detection                      
        FOXINT_ID_FD,           //Face Detection                       
        FOXINT_ID_OTD,          //Object Type Detection                
        FOXINT_ID_OCD,          //Object Color Type Detection          
                                                                       
} FoxInt_ID;                                                           

typedef enum                                      
{                                                 
        FOXINT_SID_INVALID = -1,                  
        FOXINT_SID_IN,          //People In       
        FOXINT_SID_OUT,         //People Out      
        FOXINT_SID_VL,          //Virtual Line    
                                                  
} FoxInt_SID;                                     

typedef struct                         
{                                      
        FoxInt_ID Type;                
        FoxInt_SID SubType;            
        unsigned char Group;           
                                       
} FoxInt_Type;                     

/*
typedef enum
{
	E_Intelligent_MO,	//Motion Object Detected
	E_Intelligent_FA,	//Fence Invasion Alarm
	E_Intelligent_SC,	//Scene Change Alarm
	E_Intelligent_PC,	//People Count Change
	E_Intelligent_DD,	//Direction Detected
	E_Intelligent_UO,	//Unattached Object Alarm
	E_Intelligent_MS,	//Missing Object Detection
	E_Intelligent_FD,	//Face Detected
	E_Intelligent_CD,	//Crowd Detected
	E_Intelligent_LR,	//License Plate Recoginition
	E_Intelligent_OT,	//Object Type Detection
	E_Intelligent_OC,	//Object Color Type Detection

} E_Intelligent_ID;
*/

#define E_Intelligent_MO        0x51   
#define E_Intelligent_FA        0x21   
#define E_Intelligent_SC        0x22   
#define E_Intelligent_PC        0x28   
#define E_Intelligent_DD        0x24   
#define E_Intelligent_UO        0x54   
#define E_Intelligent_MS        0x52   
#define E_Intelligent_FD        0x58   
#define E_Intelligent_CD        0x61   
//#define E_Intelligent_LR             
#define E_Intelligent_OT        0x32	//���]��     
#define E_Intelligent_OC        0x31   

/*********************************
  fence invasion alarm data
 **********************************/
typedef struct _IpcEventDataIntelligent_FAData
{
        //Byte Index | EventID Mask                      
        unsigned char   EventID;                         
        //8bits,bit 0: 1'st fence......bit 7: 8'rd fence 
        unsigned char   FenceAlarm;                      

} IpcEventDataIntelligent_FAData;


/*********************************
  Direction Detected alarm data
 **********************************/
typedef struct _IpcEventDataIntelligent_DDData
{
        //Byte Index | EventID Mask
        unsigned char   EventID;   
        unsigned char   Reserved;  
	//16bits,
	//bit 0: walk in alarm of 1��st Counting Group
	//bit 1: walk out alarm of 1��st Counting Group
	//and so on, Maximum group of Direction Data is 8 group (16bits)
	unsigned short	DirectionData;			

} IpcEventDataIntelligent_DDData;


/*********************************
  People counting data
 **********************************/
typedef struct _IpcEventDataIntelligent_PCData
{
        //Byte Index | EventID Mask
        unsigned char   EventID;   
	//Time Distance(Minutes) | Door ID
        unsigned char   DoorID;  

	unsigned char	nIn;
	unsigned char	nOut;

} IpcEventDataIntelligent_PCData;


/*********************************
  Object Type Detection / Object Color Type Detection Data
 **********************************/
/*
typedef struct _IpcEventDataIntelligent_OTDData
{
	unsigned int	nIn[MAX_PC_GROUP];
	unsigned int	nOut[MAX_PC_GROUP];
	unsigned char 	Type;		//ObjecsSizeType : TINY/SMALL/MEDIUM/LARGE/HUGE

} IpcEventDataIntelligent_OTDData;
*/

/*********************************
  Object Color Type Detection data
 **********************************/
/*
typedef struct _IpcEventDataIntelligent_OCDData
{
	unsigned int 	nIn[MAX_PC_GROUP];
	unsigned int 	nOut[MAX_PC_GROUP];
	unsigned char	ColorType;

} IpcEventDataIntelligent_OCDData;
*/

/*********************************
  Object Type Detection / Object Color Type Detection Data
 **********************************/
typedef struct _IpcEventDataIntelligent_TypeHeader
{
	//Byte Index | EventID Mask
	unsigned char	EventID;

	//Number of Data
	unsigned char	nNumOfData;

} IpcEventDataIntelligent_TypeHeader;

typedef struct _IpcEventDataIntelligent_TypeData
{
	//Region ID(Group ID) | Type
	unsigned char	Group_Type;
	
	//Object In | Object Out Count
	unsigned char 	nInOut;

} IpcEventDataIntelligent_TypeData;

/*********************************
  Object Data for MO / UO / MS / FD
 **********************************/
typedef struct _IpcEventDataIntelligent_ObjData
{
	//Byte Index | EventID Mask
	unsigned char 	EventID;

	unsigned char	nId;		//id of object
	unsigned short	nXoff;		//x-offset
	unsigned short	nYoff;		//y-offset
	unsigned short	nWidth;		//width
	unsigned short	nHeight;	//height

} IpcEventDataIntelligent_ObjData;

/*********************************
  LPR Data
 **********************************/
typedef struct _IpcEventDataIntelligent_LPRData
{
	unsigned int	nId;		//id of object
	unsigned char	nXoff;		//x-offset
	unsigned char	nYoff;		//y-offset
	unsigned char	nWidth;		//width
	unsigned char	nHeight;	//height
	char		License[16];

} IpcEventDataIntelligent_LPRData;

/*********************************
  Scene Change Alarm
 **********************************/
typedef struct _IpcEventDataIntelligent_SCData
{
	//Byte Index | EventID Mask
	unsigned char EventID;

	//bit3 : Lens Focus Loss
	//bit2 : Device Moving
	//bit1 : Video Blind
	//bit0 : Video Loss
	unsigned char SCAlarm;

} IpcEventDataIntelligent_SCData;

/*********************************
  Crowd Detection
 **********************************/
typedef struct _IpcEventDataIntelligent_CDData
{
	//Byte Index | EventID Mask
	unsigned char EventID;

	unsigned char Crow;

} IpcEventDataIntelligent_CDData;

/*********************************
  Intelligent Data Header
 **********************************/
typedef struct _IpcEventDataIntelligent
{
	unsigned char	ch;
	int		nLen;		//size of total Intelligence data
	int		nNumOfData;	//number of Intelligence data
	char		FName[32];	//JPEG File

} IpcEventDataIntelligent;

#endif /* IVAI_PARAMETERS_H_ */
