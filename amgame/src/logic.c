#include <game.h>
static pixel gradient(pixel,pixel,int);
void init_screen(void);
void swap_pixel(void);
int print_flag=1;
void init(void){
//initialization
  cursor_x=0,cursor_y=0;
  choosen_idx=0;
  stat=GAME_PLAYING;
  pixel rd,ld,ru,lu;
  rd.val=0x00000000;//Right Down
  ld.val=0x00ff0000;//Left  Down
  ru.val=0x0000ff00;//Right Up
  lu.val=0x000000ff;//Left  Up
//fill colors and indexes
  for(int x=0;x<GRID_NUM;++x){
    pixel left=gradient(ld,lu,x);
    pixel right=gradient(rd,ru,x);
    for (int y=0;y<GRID_NUM;++y){
      color[x][y]=gradient(left,right,y);
      idx[x][y]=(x<<3)+y;
    }
  }
//fix grids
  color[0][0].alpha=1;//Use alpha=1
  color[GRID_NUM-1][GRID_NUM-1].alpha=1;//to fix
  color[0][GRID_NUM-1].alpha=1;//specific
  color[GRID_NUM-1][0].alpha=1;//grids
//draw help
  draw_str("Move with arrow keys\n"
           "Select with space key\n",0,0,SIDE/8,0x3fff00);
  draw_str("Swap to\nput the\ncolors\nin order!",w-14*8,8*8,SIDE/8,0x3fff00);
  draw_str("Press h for hints",0,h-2*8,SIDE/8,0x3fff00);
//random shuffle
  for(int i=0;i<SHUFFLE_TIMES;++i){
      int j,k;
      //prevent shuffle fixed grids
      do{
        j=rand()%(GRID_NUM*GRID_NUM);
      }while(color[j/GRID_NUM][j%GRID_NUM].alpha==1);
      do{
        k=rand()%(GRID_NUM*GRID_NUM);
      }while(color[k/GRID_NUM][k%GRID_NUM].alpha==1);

      //swap color
      {pixel temp=color[j/GRID_NUM][j%GRID_NUM];
      color[j/GRID_NUM][j%GRID_NUM]=color[k/GRID_NUM][k%GRID_NUM];
      color[k/GRID_NUM][k%GRID_NUM]=temp;}
      //swap index
      {uint8_t temp=idx[j/GRID_NUM][j%GRID_NUM];
      idx[j/GRID_NUM][j%GRID_NUM]=idx[k/GRID_NUM][k%GRID_NUM];
      idx[k/GRID_NUM][k%GRID_NUM]=temp;}
  }
//print colors
  for (int x = 0; x<GRID_NUM; x ++) {
    for (int y = 0; y<GRID_NUM; y++) {
      draw_grid(x,y);
    }
  }
}
void swap_pixel(void){
//swap color
  {pixel temp=color[choosen[0]][choosen[1]];
  color[choosen[0]][choosen[1]]=color[cursor_x][cursor_y];
  color[cursor_x][cursor_y]=temp;}
//swap index
  {uint8_t temp=idx[choosen[0]][choosen[1]];
  idx[choosen[0]][choosen[1]]=idx[cursor_x][cursor_y];
  idx[cursor_x][cursor_y]=temp;}
//redraw
  draw_grid(choosen[0],choosen[1]);
  draw_grid(cursor_x,cursor_y);
  int i,j;
  for(i=0;i<GRID_NUM;++i){
      for(j=0;j<GRID_NUM;++j){
          if(idx[i][j]!=(i*GRID_NUM+j))return;
      }
  }
  if((i==GRID_NUM)&&(j==GRID_NUM)){
    stat=GAME_WIN;
    for(int k=0;k<6;++k){
        mono_rect(w/2+(k*3-13)*SIDE,h/2-(SIDE*5)/2,SIDE*3,SIDE*6,0xffffff);
    }
    draw_str("You Win!\n"
            "press r to restart",w/2-SIDE*12,h/2-2*SIDE,SIDE/8,0x3fff00);
    draw_str("Too simple?\n"
            "Edit include/game.h",w/2-SIDE*12,h/2+SIDE,SIDE/8,0x3fff00);
  }
}

static pixel gradient(pixel a,pixel b,int i){
    pixel temp;
    temp.val=0;
    temp.r=((b.r-a.r)*i)/GRID_NUM+a.r;
    temp.g=((b.g-a.g)*i)/GRID_NUM+a.g;
    temp.b=((b.b-a.b)*i)/GRID_NUM+a.b;
    return temp;
}

void operate(int key){
    if(key&0x8000){
      switch(stat){
        case GAME_PLAYING:
          draw_grid(cursor_x,cursor_y);
          switch(key^0x8000){
            case _KEY_SPACE:
              if(color[cursor_x][cursor_y].alpha==1)break;
              draw_circle(cursor_locat,w/40,0xffffff,0x00000000);
              if(choosen_idx==0){choosen_idx=1;choosen[0]=cursor_x;choosen[1]=cursor_y;}
              else{choosen_idx=0;swap_pixel();}
              break;
            case _KEY_RIGHT:
              ++cursor_x;if(cursor_x>=GRID_NUM)cursor_x-=GRID_NUM;break;
            case _KEY_LEFT:
              --cursor_x;if(cursor_x<0)cursor_x+=GRID_NUM;break;
            case _KEY_DOWN:
              ++cursor_y;if(cursor_y>=GRID_NUM)cursor_y-=GRID_NUM;break;
            case _KEY_UP:
              --cursor_y;if(cursor_y<0)cursor_y+=GRID_NUM;break;
            case _KEY_H:
              print_flag=!print_flag;
              break;
            case _KEY_T:
              {
                  int i,j;
                  for(i=0;i<GRID_NUM;++i){
                      for(j=0;j<GRID_NUM;++j){
                          printf("%d,",idx[j][i]);
                      }
                      printf("\n");
                  }
              }
            default:
              break;
          }
        break;
        case GAME_WIN:
          if(key==(_KEY_R|0x8000)){
            init();
          }
          break;
        case GAME_PAUSE:
        default:
          printf("\nStat not handle!\n");
      }
    }
}
int next_key(void){
    static int old_key=_KEY_NONE,old_time=0;
    int new_key=read_key(),new_time=uptime();
    //delay for same key pressing
    if(new_time-old_time>50){
        old_time=new_time;
        return old_key=new_key;
    }else{
        if(new_key!=old_key&&new_key!=_KEY_NONE){
            old_time=new_time;
            return old_key=new_key;
        }else{
            return _KEY_NONE;
        }
    }
}
