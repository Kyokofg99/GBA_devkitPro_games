#include <string.h>
#include <tonc_video.h>
#include <tonc_input.h>
#include <tonc_types.h>

#include "ball.h"
#include "paddle.h"

#include "img/ball_sprite.h"
#include "img/ball_sprite.c"
#include "img/paddle_sprite.h"
#include "img/paddle_sprite.c"
#include "img/pong_paddle.h"
#include "img/pong_paddle.c"


bool check_collisions(struct paddle* paddle, struct ball* ball)
{
	if (paddle->x < ball->x+ball->size && paddle->x+paddle->width > ball->x)
	{
		if (paddle->y < ball->y+ball->size && paddle->y+paddle->height > ball->y)
        {
			return true;
        }
	}
	return false;
}

bool check_collisions_next_frame(struct paddle* paddle, struct ball* ball)
{
	if ((paddle->x  <  ball->x + ball->x_direction*ball->x_speed + ball->size)   
        &&  (paddle->x + paddle->width  >  ball->x + ball->x_direction*ball->x_speed))
	{
        /* TODO Para tratar que la pala puede no estar moviendose, puedo usas min() y max() */
		if ((paddle->y - paddle->speed /*- 8*/  <  ball->y + ball->size + ball->y_speed /*+ 8*/)  
            &&  (paddle->y+paddle->height+paddle->speed /*+ 8*/ >  ball->y - ball->y_speed /*- 8*/))
        {
			return true;
        }
	}
	return false;
}

void clamp_right(struct paddle* paddle, struct ball* ball)
{
    if ((ball->x >= paddle->x+paddle->width) || (ball->x+ball->size <= paddle->x))
        ball->x = paddle->x+paddle->width+ball->size;
}
void clamp_left(struct paddle* paddle, struct ball* ball)
{
    if ((ball->x >= paddle->x+paddle->width) || (ball->x+ball->size <= paddle->x))
        ball->x = paddle->x-ball->size;
}

int main()
{
    memcpy(pal_obj_mem, pong_paddlePal,  pong_paddlePalLen );
    memcpy(&tile8_mem[4][1], ball_spriteTiles, ball_spriteTilesLen);

    //memcpy(pal_obj_mem, paddle_spritePal,  paddle_spritePalLen );
    memcpy(&tile8_mem[4][5], pong_paddleTiles, pong_paddleTilesLen);

    struct ball ball;
    init_ball(&ball, SCREEN_WIDTH >> 1, SCREEN_HEIGHT >> 1, 8, 1, &oam_mem[0]);

    struct paddle paddle;
    init_paddle(&paddle, 16, SCREEN_HEIGHT >> 1, 8, 32, 3, &oam_mem[1]);

    struct paddle paddle_enemy;
    init_paddle(&paddle_enemy, SCREEN_WIDTH - 16 - 8, ball.y, 8, 32, 3, &oam_mem[2]);
    paddle_enemy.obj_attributes->attr1 |= 0x3000;



    REG_DISPCNT =  DCNT_MODE0 | DCNT_OBJ | DCNT_OBJ_1D;

    

    while(1)
    {
        // wait vblank
        vid_vsync();

        key_poll();

        // update
        move_ball(&ball);

        paddle.y_direction = 0;
        paddle.speed = 0;
        if (key_is_down(KEY_DOWN))
        {
            paddle.y_direction = 1;
            paddle.speed = 3;
        }
        else if (key_is_down(KEY_UP))
        {
            paddle.y_direction = -1;
            paddle.speed = 3;
        }

        if (ball.x_collision_immunity_countdown > 0 || ball.y_collision_immunity_countdown > 0)
            paddle.speed = 0;
        
        move_paddle(&paddle);

        move_paddle_by_value(&paddle_enemy, ball.y_direction);

        /* If ball collisioned with paddle recently, decrease countdown. */
        if (ball.x_collision_immunity_countdown > 0)
		    ball.x_collision_immunity_countdown--;
        if (ball.y_collision_immunity_countdown > 0)
		    ball.y_collision_immunity_countdown--;

        /* Process paddle <-> ball collision */
        if (ball.x_collision_immunity_countdown > 0 && ball.y_collision_immunity_countdown > 0)
            ;
        else if (check_collisions_next_frame(&paddle, &ball))
        {
            /* if paddle-top is below ball_bot  */
            if ((paddle.y >= ball.y+ball.size) /*&& (ball.y+ball.size+ball.y_direction > paddle.y)*/)
            {
                if (ball.y_collision_immunity_countdown == 0)
                    bounce_ball_y(&ball, 1+ABS(paddle.y_direction)*1);
                ball.y = MIN(ball.y, paddle.y-ball.size - paddle.speed -ball.y_speed /*- 4*/);
                ball.y_collision_immunity_countdown = 10;
            } /* if paddle_bot is above */
            else if (paddle.y+paddle.height <= ball.y)
            {
                if (ball.y_collision_immunity_countdown == 0)
                    bounce_ball_y(&ball, 1+ABS(paddle.y_direction)*1);
                ball.y = MAX(ball.y, paddle.y + paddle.height + paddle.speed + ball.y_speed /*+ 4*/);
                ball.y_collision_immunity_countdown = 10;
            }
            else
            {
                if (ball.x_collision_immunity_countdown == 0)
                    bounce_ball_test(&ball, true, false, 0, 0);
                if (paddle.y_direction)
                    ball.y_direction = paddle.y_direction;
                //ball.x_collision_immunity_countdown = 60;
            }
        }

        /* Process paddle_enemy <-> ball collision */
        if (check_collisions_next_frame(&paddle_enemy, &ball))
        {
            bounce_ball(&ball);
        }

        // draw
        draw_ball(&ball);
        draw_paddle(&paddle);
        draw_paddle(&paddle_enemy);
    }

    return 0;
}