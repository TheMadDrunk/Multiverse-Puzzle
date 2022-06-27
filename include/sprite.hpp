#ifndef SPRITE
#define SPRITE

#include "raylib.h"
#include <vector>
#include <iostream>  

//I love c++
Vector2 operator+(const Vector2& left,const Vector2& right){
    return Vector2{left.x+right.x,left.y+right.y};
}
Vector2 operator-(const Vector2& left,const Vector2& right){
    return Vector2{left.x-right.x,left.y-right.y};
}
Vector2 operator*(const float& left,const Vector2& right){
    return Vector2{right.x*left,right.y*left};
}
void printV2(Vector2& v2,std::string str=""){
    std::cout<<str+' '<<v2.x<<" - "<<v2.y<<'\n';
}


class Animation
{
private:
    int curr_frame;
    double time_last_frame;
public:
    //set speed
    int nbr_frames;
    double speed;
    //texture
    Texture spriteSheet;
    //le pas x,y
    Vector2 start_pos;
    Vector2 step;
    Vector2 size;
    
    Animation(Texture spriteSheet,Vector2 step,Vector2 start_pos,Vector2 size,double speed,int nbr_frames){
        this->speed = speed;
        this->spriteSheet = spriteSheet;
        this->step = step;
        this->nbr_frames = nbr_frames;
        this->start_pos = start_pos;
        this->curr_frame=0;
        this->time_last_frame = GetTime();
        this->size = size;
    }

    Animation(const Animation& anim){
        this->nbr_frames = anim.nbr_frames;
        this->speed = anim.speed;
        this->spriteSheet = anim.spriteSheet;
        this->start_pos = anim.start_pos;
        this->step = anim.step;
        this->size = anim.size;
        this->curr_frame = 0;
        this->time_last_frame = GetTime();
    }

    void draw(Vector2 draw_pos){
        
        if(GetTime()-time_last_frame>=speed/1000){
            curr_frame = (curr_frame+1)%nbr_frames;
            time_last_frame = GetTime();
        }

        Vector2 pos_frame = start_pos + (curr_frame*step);
        DrawTextureRec(spriteSheet,(Rectangle){pos_frame.x,pos_frame.y,size.x,size.y},draw_pos,WHITE);
    }
};


class Sprite
{
public:
    std::vector<Animation> animations;

    Vector2 animation_step;
    Vector2 sprites_size;

    Sprite(Vector2 animation_step,Vector2 sprites_size){
        this->animation_step = animation_step;
        this->sprites_size = sprites_size;
    }
    Sprite(){
        animation_step = Vector2{16,0};
        sprites_size = Vector2{16,16};
    }
    void addAnimation(Texture spriSheet,Vector2 start_pos,double speed,int nbr_frames){
        animations.push_back(Animation(spriSheet,animation_step,start_pos,sprites_size,speed,nbr_frames));
    }
    void addAnimationAnim(Animation anim){
        animations.push_back(anim);
    }
    void draw(int animation_index,Vector2 draw_pos){
        animations[animation_index].draw(draw_pos);
    }

    void draw_centred(int animation_index,Vector2 pos_centred){
        Vector2 draw_pos = pos_centred - 0.5*animations[animation_index].size;
        animations[animation_index].draw(draw_pos);
    }
};



#endif //SPRITE