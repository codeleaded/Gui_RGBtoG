#include "/home/codeleaded/System/Static/Library/WindowEngine1.0.h"
#include "/home/codeleaded/System/Static/Library/GSprite.h"
#include "/home/codeleaded/System/Static/Library/NeuralNetwork.h"
#include "/home/codeleaded/System/Static/Library/AffineTransform.h"

#define SPRITE_PATH         "/home/codeleaded/Data/NN/Digits/"
#define DATA_PATH           "/home/codeleaded/Data/NN/DigitsGray/"
#define SPRITE_TEST         "testing"
#define SPRITE_TRAINING     "training"
#define SPRITE_COUNT        1
#define SPRITE_MAX          5000

#define NN_PATH             "./data/Model.nnalx"
#define NN_COUNT            10
#define NN_LEARNRATE        0.5f

unsigned int ndir = 0U;
unsigned int item = 0U;
GSprite sp;

GSprite GSprite_New_Transformed(GSprite* sp,float x,float y,float s,float a){
    AffineTransform transform = AffineTransform_New();
    AffineTransform_Identity(&transform);
    
    AffineTransform_Trans(&transform,(Vec2){ -(float)sp->w * 0.5f,-(float)sp->h * 0.5f });
    AffineTransform_RotateZ(&transform,a);
    AffineTransform_Trans(&transform,(Vec2){ (float)sp->w * 0.5f,(float)sp->h * 0.5f });

    AffineTransform_Trans(&transform,(Vec2){ x,y });
    AffineTransform_Scale(&transform,(Vec2){ s,s });

    AffineTransform_Inverse(&transform);
    
    GSprite transformed = GSprite_New(sp->w,sp->h);
    memset(transformed.img,0,sizeof(float) * transformed.w * transformed.h);
    
    for(int i = 0;i<transformed.h;i++){
        for(int j = 0;j<transformed.w;j++){
            const Vec2 rp = { j,i };
            const Vec2 p = AffineTransform_Calc(&transform,rp);
            const float l = GSprite_Get(sp,p.x,p.y);
            if(l>=0.0f) transformed.img[i * transformed.w + j] = l;
        }
    }

    return transformed;
}

void Crawler_Build(char* path){
    for(int i = 0;i<NN_COUNT;i++){
        for(int j = 0;j<SPRITE_MAX;j++){
            CStr filename = CStr_Format("%s/%d/%d.sprg",path,i,j);
            if(!Files_isFile(filename)){
                unsigned int item = Random_u32_MinMax(0,j);

                CStr randfilename = CStr_Format("%s/%d/%d.sprg",path,i,item);
                GSprite sp = GSprite_Load(randfilename);

                const float x = (float)Random_f64_MinMax(-(double)sp.w * 0.05,(double)sp.w * 0.05);
                const float y = (float)Random_f64_MinMax(-(double)sp.h * 0.05,(double)sp.h * 0.05);
                const float s = (float)Random_f64_MinMax(0.75,1.25);
                const float a = (float)Random_f64_MinMax(-F64_PI025,F64_PI025);
                GSprite trans = GSprite_New_Transformed(&sp,x,y,s,a);
                GSprite_Save(&trans,filename);
                GSprite_Free(&trans);

                GSprite_Free(&sp);
                CStr_Free(&randfilename);
            }else{
                // Deleting new files
                //if(j >= 300) Files_Remove(filename);
            }
            CStr_Free(&filename);
        }
    }
}

void Setup(AlxWindow* w){
    RGA_Set(Time_Nano());

    ResizeAlxFont(8,8);

    Crawler_Build(DATA_PATH SPRITE_TRAINING);
    Crawler_Build(DATA_PATH SPRITE_TEST);
}
void Update(AlxWindow* w){
    if(Stroke(ALX_KEY_R).PRESSED){
        ndir = Random_u32_MinMax(0,10);
        item = Random_u32_MinMax(0,SPRITE_MAX);

        CStr ntraining_s = CStr_Format("%s/%d/%d.sprg",DATA_PATH SPRITE_TRAINING,ndir,item);
        GSprite_Free(&sp);
        sp = GSprite_Load(ntraining_s);
        CStr_Free(&ntraining_s);
    }
    if(Stroke(ALX_KEY_T).PRESSED){
        ndir = Random_u32_MinMax(0,10);
        item = Random_u32_MinMax(0,SPRITE_MAX);

        CStr ntraining_s = CStr_Format("%s/%d/%d.sprg",DATA_PATH SPRITE_TEST,ndir,item);
        GSprite_Free(&sp);
        sp = GSprite_Load(ntraining_s);
        CStr_Free(&ntraining_s);
    }
    
    Clear(DARK_BLUE);

    GSprite_Render(WINDOW_STD_ARGS,&sp,10.0f,10.0f);

    //String str = String_Format("T:%d, ND:%d, I:%d",test,ndir,item);
    //RenderCStrSize(str.Memory,str.size,0.0f,0.0f,WHITE);
    //String_Free(&str);

    String str = String_Format("N: %u, I: %u",ndir,item);
    RenderCStrSize(str.Memory,str.size,0.0f,GetHeight() - GetAlxFont()->CharSizeY,WHITE);
    String_Free(&str);
}
void Delete(AlxWindow* w){
    GSprite_Free(&sp);
}

int main(){
    if(Create("RGB to G",160,120,8,8,Setup,Update,Delete))
        Start();
    return 0;
}