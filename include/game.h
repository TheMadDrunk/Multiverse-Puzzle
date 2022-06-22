#ifndef GAME
#define GAME

#include <algorithm>
#include <vector>
#include "raylib.h"
#include "sprite.h"
#include "level.h"
#include <iostream>

#define TILE_SIZE 16

const int tilePlayer = 1,tileHeart = 16,tileRobot1 = 7,tileRobot2 = 13,tileActive1 = 6,tileActive2 = 12,tileDecale = 17,tileFree=23;

Color defau = Color{50,24,90,1};
Color yellow = Color{78,78,23,1};


class movable
{
    public:
    Vector2 position;
    Vector2 lastpos;

    std::vector<int> obstacles;

    movable(){
        position = Vector2{0,0};
        lastpos = position;
    }

    movable(Vector2 pos):position(pos){}

    movable(Vector2 pos,std::vector<int>& vec):obstacles(vec),position(pos){}

    void move_left(std::vector<std::vector<int>>& maplevel){

        if(position.y-1<0)
            return;
        
        auto it = std::find(obstacles.begin(),obstacles.end(),maplevel[position.x][position.y-1]);
        
        if(it == obstacles.end())
            position.y-=1;
    }

    void move_right(std::vector<std::vector<int>>& maplevel){
        int n = maplevel.size(),m = maplevel[0].size();

        if(position.y+1>=n)
            return;
        auto it = std::find(obstacles.begin(),obstacles.end(),maplevel[position.x][position.y+1]);
        if(it == obstacles.end())
            position.y+=1;
    }

    void move_up(std::vector<std::vector<int>>& maplevel){
        int n = maplevel.size(),m = maplevel[0].size();

        if(position.x-1<0)
            return;
        auto it = std::find(obstacles.begin(),obstacles.end(),maplevel[position.x-1][position.y]);
        if(it == obstacles.end())
            position.x-=1;
    }

    void move_down(std::vector<std::vector<int>>& maplevel){
        int n = maplevel.size(),m = maplevel[0].size();
       
        if(position.x+1>=m)
            return;
        auto it = std::find(obstacles.begin(),obstacles.end(),maplevel[position.x+1][position.y]);
        
        if(it == obstacles.end())
            position.x+=1;
        
    }

    void updat_pos(std::vector<std::vector<int>>& maplevel,int id){
        maplevel[lastpos.x][lastpos.y] = 0;
        maplevel[position.x][position.y] = id;
        lastpos = position;
        //std::cout<<position.x<<" "<<position.y<<'\n';
    }
    
};

struct levelData
{
    Level level;
    movable player;
    std::vector<movable> robots;
    std::vector<int> input_inc;
    std::vector<bool> enabled;
    Color bg_color;
    std::vector<Vector2> hearts;
    std::vector<int> tiles;
    int last_mov;
    int maxMoves;
    int moves_counter;
    int cam_scale;
};

void update_player(levelData& lvldt,Sound* move,Sound* active,Sound* deactive){
    if(IsKeyPressed(KEY_LEFT)){
        lvldt.player.move_left(lvldt.level.matrix[LAYER_ONE]);
        lvldt.moves_counter++;
        PlaySound(*move);
        lvldt.last_mov = KEY_LEFT;
    }
    if(IsKeyPressed(KEY_RIGHT)){
        lvldt.player.move_right(lvldt.level.matrix[LAYER_ONE]);
        lvldt.moves_counter++;
        PlaySound(*move);
        lvldt.last_mov = KEY_RIGHT;
    }
    if(IsKeyPressed(KEY_UP)){
        lvldt.player.move_up(lvldt.level.matrix[LAYER_ONE]);
        lvldt.moves_counter++;
        PlaySound(*move);
        lvldt.last_mov = KEY_UP;
    }
    if(IsKeyPressed(KEY_DOWN)){
        lvldt.player.move_down(lvldt.level.matrix[LAYER_ONE]);
        lvldt.moves_counter++;
        PlaySound(*move);
        lvldt.last_mov = KEY_DOWN;
    }
    //std::cout<<lvldt.player.position.x<<" "<<lvldt.player.position.y<<'\n';
    
    //check if on enable tile (layer2)
    //check if it the same tile
    if(lvldt.player.position.x != lvldt.player.lastpos.x or lvldt.player.position.y != lvldt.player.lastpos.y ){

    if(lvldt.level.matrix[LAYER_TWO][lvldt.player.position.x][lvldt.player.position.y] == tileActive1){
        lvldt.enabled[0] = !lvldt.enabled[0];
        if(lvldt.enabled[0] and !IsSoundPlaying(*active))
            PlaySound(*active);
        if(!lvldt.enabled[0] and !IsSoundPlaying(*deactive))
            PlaySound(*deactive);
    } 

    if(lvldt.level.matrix[LAYER_TWO][lvldt.player.position.x][lvldt.player.position.y] == tileActive2){
        lvldt.enabled[1] = !lvldt.enabled[1];
        if(lvldt.enabled[1] and !IsSoundPlaying(*active))
            PlaySound(*active);
        if(!lvldt.enabled[1] and !IsSoundPlaying(*deactive))
            PlaySound(*deactive);
    }    
    }
    lvldt.player.updat_pos(lvldt.level.matrix[LAYER_PLAYER],1);

}

void update_robots(levelData& lvldt,Sound* decale){
    
    for (int i = 0;lvldt.last_mov!=-1 and i < lvldt.robots.size(); i++)
    {   
        if(!lvldt.enabled[i])            
            continue;

        int mov = (lvldt.last_mov+lvldt.input_inc[i]-262)%4 + 262; 
        switch (mov)
        {
        case KEY_UP:
            lvldt.robots[i].move_up(lvldt.level.matrix[LAYER_ONE]);
            break;
        case KEY_DOWN:
            lvldt.robots[i].move_down(lvldt.level.matrix[LAYER_ONE]);
            break;
        case KEY_LEFT:
            lvldt.robots[i].move_left(lvldt.level.matrix[LAYER_ONE]);
            break;
        case KEY_RIGHT:
            lvldt.robots[i].move_right(lvldt.level.matrix[LAYER_ONE]);
            break;
        
        default:
            std::cout<<"robot input problem\n";
            break;
        }

        if(lvldt.level.matrix[LAYER_TWO][lvldt.robots[i].position.x][lvldt.robots[i].position.y] == tileDecale){
            lvldt.input_inc[i]+=1;
            if(!IsSoundPlaying(*decale))
                PlaySound(*decale);
        }

    }
    lvldt.last_mov = -1;
    
    for(int i = 0;i < lvldt.robots.size(); i++){
        lvldt.robots[i].updat_pos(lvldt.level.matrix[LAYER_THREE],lvldt.tiles[i]);
    }

}

bool update_level(levelData& lvldt,int* lvl_cnt,Sound* vic,Sound* nvic){
    
    //check if win
    bool flag_win = true;
    for(int i = 0;i<lvldt.hearts.size();i++){
        Vector2 pos = lvldt.hearts[i];
        int tile_id3 = lvldt.level.matrix[LAYER_THREE][pos.x][pos.y];
        int tile_id4 = lvldt.level.matrix[LAYER_PLAYER][pos.x][pos.y];
        if(tile_id3 == 0 and tile_id4==0){
            flag_win = false;
            break;
        }
    }

    if(flag_win and lvldt.moves_counter<=lvldt.maxMoves){
        if(!IsSoundPlaying(*vic))
            PlaySound(*vic);
        std::cout<<"WIN!\n";
        (*lvl_cnt)++;
        return true;
    }
    if(flag_win and !IsSoundPlaying(*nvic) and lvldt.moves_counter>lvldt.maxMoves){
        PlaySound(*nvic);
    }

    return false;
}

Camera2D update_cam(levelData& lvldt,int width,int height){
    Camera2D cam;
    cam.target = Vector2{0};
    cam.offset = Vector2{(float) width/2-(lvldt.level.width*lvldt.cam_scale)/2,
                         (float) height/2-(lvldt.level.height*lvldt.cam_scale)/2 +25};
    cam.zoom = lvldt.cam_scale;
    cam.rotation = 0;

    return cam;
}

void dostuff(std::vector<Vector2>& arr,std::vector<std::vector<int>>& matrix,int tile_id){
    int n = matrix.size(),m = matrix[0].size();
        for(int i = 0;i<n;i++)
            for(int j = 0;j<m;j++){
                if(matrix[i][j] == tile_id)
                    arr.push_back(Vector2{(float)i,(float)j});
            }
}
void dostuff(Vector2* vec,std::vector<std::vector<int>>& matrix,int tile_id){
    int n = matrix.size(),m = matrix[0].size();
    
        for(int i = 0;i<n;i++)
            for(int j = 0;j<m;j++){
                  
                if(matrix[i][j] == tile_id){
                    *vec = Vector2{(float)i,(float)j};
                    break;
                }
            }
}

void init(int level,levelData* newlvl){
    std::cout<<"init level\n";
    *newlvl = levelData{}; 
    std::cout<<"init level\n";
    std::vector<int> obstcl= {0,5,11};
    newlvl->robots.resize(2,movable(Vector2{0},obstcl));
    newlvl->input_inc.resize(2,0);
    newlvl->enabled.resize(2,true);
    newlvl->tiles.resize(2,tileFree);
    switch (level)
    {
    case 0:
        newlvl->level.LoadLevel("level1.txt");
        newlvl->player = movable(Vector2{13,3},obstcl);
        
        dostuff(&(newlvl->robots[0].position),newlvl->level.matrix[LAYER_THREE],tileRobot1);
        newlvl->tiles[0] = tileRobot1;
        dostuff(newlvl->hearts,newlvl->level.matrix[LAYER_TWO],tileHeart);
        newlvl->bg_color = Color{50,24,90,1};
        newlvl->last_mov = -1;
        newlvl->maxMoves = 19;
        newlvl->moves_counter = 0;
        newlvl->cam_scale = 2;
        break;
    case 1:
        newlvl->level.LoadLevel("level2.txt");
        //player
        newlvl->player = movable(Vector2{0,0},obstcl);
        dostuff(&(newlvl->player.position),newlvl->level.matrix[LAYER_PLAYER],tilePlayer);
        //robot1

        dostuff(&(newlvl->robots[0].position),newlvl->level.matrix[LAYER_THREE],tileRobot1);
        newlvl->tiles[0] = tileRobot1;

        //robot2

        //hearts
        dostuff(newlvl->hearts,newlvl->level.matrix[LAYER_TWO],tileHeart);

        newlvl->bg_color = Color{50,24,90,1};
        newlvl->last_mov = -1;
        //max moves
        newlvl->maxMoves = 26;
        newlvl->moves_counter = 0;
        newlvl->cam_scale = 2; 
        break;
    case 2:
        newlvl->level.LoadLevel("level3.txt");
        //player
        newlvl->player = movable(Vector2{0,0},obstcl);
        dostuff(&(newlvl->player.position),newlvl->level.matrix[LAYER_PLAYER],tilePlayer);
        //robot1
        /*
        dostuff(&(newlvl->robots[0].position),newlvl->level.matrix[LAYER_THREE],tileRobot1);
        */
        //robot2
        
        dostuff(&(newlvl->robots[1].position),newlvl->level.matrix[LAYER_THREE],tileRobot2);
        newlvl->tiles[1] = tileRobot2;

        
        //hearts
        dostuff(newlvl->hearts,newlvl->level.matrix[LAYER_TWO],tileHeart);
        newlvl->bg_color = yellow;
        newlvl->last_mov = -1;
        //max moves
        newlvl->maxMoves = 21;
        newlvl->moves_counter = 0;
        newlvl->cam_scale = 2; 
        break;
    case 3:
        newlvl->level.LoadLevel("level4.txt");
        //player
        newlvl->player = movable(Vector2{0,0},obstcl);
        dostuff(&(newlvl->player.position),newlvl->level.matrix[LAYER_PLAYER],tilePlayer);
        //robot1
        
        dostuff(&(newlvl->robots[0].position),newlvl->level.matrix[LAYER_THREE],tileRobot1);
        newlvl->tiles[0] = tileRobot1;
        //robot2
        
        dostuff(&(newlvl->robots[1].position),newlvl->level.matrix[LAYER_THREE],tileRobot2);
        newlvl->tiles[1] = tileRobot2;

        
        //hearts
        dostuff(newlvl->hearts,newlvl->level.matrix[LAYER_TWO],tileHeart);
        newlvl->bg_color = yellow;
        newlvl->last_mov = -1;
        //max moves
        newlvl->maxMoves = 21;
        newlvl->moves_counter = 0;
        newlvl->cam_scale = 2;  
        break;
    case 4:
        newlvl->level.LoadLevel("level5.txt");
        //player
        newlvl->player = movable(Vector2{0,0},obstcl);
        dostuff(&(newlvl->player.position),newlvl->level.matrix[LAYER_PLAYER],tilePlayer);
        //robot1
        
        dostuff(&(newlvl->robots[0].position),newlvl->level.matrix[LAYER_THREE],tileRobot1);
        newlvl->tiles[0] = tileRobot1;
        
        //robot2
        /*
        dostuff(&(newlvl->robots[1].position),newlvl->level.matrix[LAYER_THREE],tileRobot2);
        newlvl->tiles[1] = tileRobot2;
        */
        
        //hearts
        dostuff(newlvl->hearts,newlvl->level.matrix[LAYER_TWO],tileHeart);
        newlvl->bg_color = defau;
        newlvl->last_mov = -1;
        //max moves
        newlvl->maxMoves = 27;
        newlvl->moves_counter = 0;
        newlvl->cam_scale = 2; 
        break;
    case 5:
        newlvl->level.LoadLevel("level6.txt");
        newlvl->player = movable(Vector2{0,0},obstcl);
        dostuff(&(newlvl->player.position),newlvl->level.matrix[LAYER_PLAYER],tilePlayer);
        //robot1
        /*
        dostuff(&(newlvl->robots[0].position),newlvl->level.matrix[LAYER_THREE],tileRobot1);
        newlvl->tiles[0] = tileRobot1;
        */
        //robot2
        
        dostuff(&(newlvl->robots[1].position),newlvl->level.matrix[LAYER_THREE],tileRobot2);
        newlvl->tiles[1] = tileRobot2;
        
        
        //hearts
        dostuff(newlvl->hearts,newlvl->level.matrix[LAYER_TWO],tileHeart);
        newlvl->bg_color = yellow;
        newlvl->last_mov = -1;
        //max moves
        newlvl->maxMoves = 28;
        newlvl->moves_counter = 0;
        newlvl->cam_scale = 2; 
        break;
    case 6:
        newlvl->level.LoadLevel("level7.txt");
        //player
        newlvl->player = movable(Vector2{0,0},obstcl);
        dostuff(&(newlvl->player.position),newlvl->level.matrix[LAYER_PLAYER],tilePlayer);
        //robot1
        
        dostuff(&(newlvl->robots[0].position),newlvl->level.matrix[LAYER_THREE],tileRobot1);
        newlvl->tiles[0] = tileRobot1;
        newlvl->input_inc[0] = 1;
        //robot2
        
        dostuff(&(newlvl->robots[1].position),newlvl->level.matrix[LAYER_THREE],tileRobot2);
        newlvl->tiles[1] = tileRobot2;
        newlvl->input_inc[1] = 3;
        
        //hearts
        dostuff(newlvl->hearts,newlvl->level.matrix[LAYER_TWO],tileHeart);
        newlvl->bg_color = defau;
        newlvl->last_mov = -1;
        //max moves
        newlvl->maxMoves = 11;
        newlvl->moves_counter = 0;
        newlvl->cam_scale = 2;  
        break;
        default:
        std::cout<<"level : "<<level<<'\n';
    }

}

#endif 