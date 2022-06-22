#include <iostream>
#include <vector>
#include "raylib.h"

#define RAYGUI_IMPLEMENTATION
#include "raygui.h"

#include "level.h"
#include "game.h"

#define S_WIDTH 800
#define S_HEIGHT 720

enum State {
    FIRST,
    GAMING,
    PICKLEVEL
    }appState;

struct lvlel
{
    movable m_movecomp;
    int id;
}player,robot;

struct lvl_btn
{
    Rectangle rec;
    std::string text;
};


int main(){
    
    InitWindow(S_WIDTH,S_HEIGHT,"Multiverse Puzzle");
    SetTargetFPS(60);

    GuiEnable();

//    GuiLoadStyle("style.rgs");
    
  //  GuiSetFont(Font{0});
    //GuiLoadStyleLavanda();

    InitAudioDevice();

    Sound victory = LoadSound("sounds/Win.mp3");
    SetSoundVolume(victory,0.15);

    Sound move_sound = LoadSound("sounds/Xmove.mp3");
    SetSoundVolume(move_sound,0.04);

    Sound cant_win = LoadSound("sounds/Damage.mp3");
    SetSoundVolume(cant_win,0.30);

    Sound decale = LoadSound("sounds/decale.mp3");
    SetSoundVolume(decale,0.15);

    Sound active = LoadSound("sounds/activated.mp3");
    SetSoundVolume(active,0.15);

    Sound deactive = LoadSound("sounds/desactivated.mp3");
    SetSoundVolume(deactive,0.15);

    Music bgMusic = LoadMusicStream("sounds/voyage.mp3");
    SetMusicVolume(bgMusic,0.10);



    appState = FIRST;

    int width_b = 110,height_b= 60,font_size=16;

    //first view
    Rectangle PlayButton = Rectangle{S_WIDTH/2-width_b/2,S_HEIGHT/2-height_b/2,width_b,height_b};
    Rectangle TutoButton = Rectangle{S_WIDTH/2-width_b/2,S_HEIGHT/2-height_b/2 + height_b + 7,width_b,height_b};
    int show_win_tuto = 1;

    //levels view
    const int maxlvl = 7;
    std::vector<lvl_btn> btnarr = {
    {Rectangle{S_WIDTH/2-width_b/2,30,width_b,height_b},"LEVEL 1"},
    {Rectangle{S_WIDTH/2-width_b/2,130,width_b,height_b},"LEVEL 2"},
    {Rectangle{S_WIDTH/2-width_b/2,230,width_b,height_b},"LEVEL 3"},
    {Rectangle{S_WIDTH/2-width_b/2,330,width_b,height_b},"LEVEL 4"},
    {Rectangle{S_WIDTH/2-width_b/2,430,width_b,height_b},"LEVEL 5"},
    {Rectangle{S_WIDTH/2-width_b/2,530,width_b,height_b},"LEVEL 6"},
    {Rectangle{S_WIDTH/2-width_b/2,630,width_b,height_b},"LEVEL 7"}
    };

    //gaming view
    Camera2D cam;
    bool loaded = false,win = false;
    int level_count = 0;
    levelData playingLevel;
    //58
    Rectangle retry = Rectangle{750,10,40,40};
    Rectangle levels = Rectangle{660,10,70,40};
    Rectangle music = Rectangle{600,10,40,40};
    
    //controled robots
    
    PlayMusicStream(bgMusic);

    while (!WindowShouldClose())
    {
        UpdateMusicStream(bgMusic);   
        if(appState == FIRST){
            BeginDrawing();
            DrawText("MULTIVERSE\n\tPUZZLE",100,100,90,WHITE);
            ClearBackground(Color{50,24,90,1});
            if(GuiButton(PlayButton,"PLAY")){
                level_count = 0;
                loaded = false;
                appState = State::GAMING;
            }
            if(GuiButton(TutoButton,"Tutoriel"))show_win_tuto= -1;
            if(show_win_tuto== -1)
            show_win_tuto = GuiMessageBox(Rectangle{S_WIDTH/2-125,S_WIDTH/2 + 100,250,200},"TUTO","You will only need \nthe arrow keys to move.\n get all the hearts at the same time in minimum moves to win","OK!");
            EndDrawing();
        }
        if(appState == PICKLEVEL){
            BeginDrawing();
            ClearBackground(Color{50,24,90,1});

            for(int i = 0;i<maxlvl;i++){
                if(GuiButton(btnarr[i].rec,btnarr[i].text.c_str())){
                    level_count = i;
                    appState = State::GAMING;
                    loaded = false;
                }
            }

            EndDrawing();
        }
        if(appState == State::GAMING){
            BeginDrawing();

            

            if(!loaded or win){
                if(level_count>=maxlvl){
                    appState = State::FIRST;
                    continue;
                }
                init(level_count,&playingLevel);
                loaded = true;
                WaitTime(300);
            }

            ClearBackground(playingLevel.bg_color);

            if(GuiButton(retry,"#58#")){
                loaded = false;
            }

            if(GuiButton(levels,"LEVELS")){
                //go to levels view
                appState = State::PICKLEVEL;
                loaded = false;
            }

            if(GuiButton(music,"Music")){
                if(IsMusicStreamPlaying(bgMusic)){
                    PauseMusicStream(bgMusic);
                }
                else ResumeMusicStream(bgMusic);
            }

            DrawText(("LEVEL "+std::to_string(level_count+1)).c_str(),S_WIDTH/2-13,10,font_size,WHITE);

            Color clr = (playingLevel.maxMoves>=playingLevel.moves_counter)? WHITE : RED;
            DrawText(std::to_string(playingLevel.maxMoves-playingLevel.moves_counter).c_str(),100,10,font_size,clr);

            update_player(playingLevel,&move_sound,&active,&deactive);

            update_robots(playingLevel,&decale);

            win = update_level(playingLevel,&level_count,&victory,&cant_win);
            
            cam = update_cam(playingLevel,S_WIDTH,S_HEIGHT);

            playingLevel.level.DrawLevel(cam);

            EndDrawing();
        }
    
        
    }
    CloseAudioDevice();
    GuiDisable();
    CloseWindow(); 
    return 0;
}
