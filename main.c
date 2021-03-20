#include <stdio.h>
#include <allegro5/allegro.h>
#include <allegro5/allegro_primitives.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_ttf.h>
#include <allegro5/allegro_audio.h>
#include <allegro5/allegro_acodec.h>
#include <math.h>

// If defined, logs will be shown on console and written to file.
// If commented out, logs will not be shown nor be saved.
#define LOG_ENABLED

/* Constants. */
int i, j;
// Frame rate (frame per second)
const int FPS = 60;
// Display (screen) width.
const int SCREEN_W = 800;
// Display (screen) height.
const int SCREEN_H = 600;
// At most 8 audios can be played at a time.
const int RESERVE_SAMPLES = 8;
// Same as:
// const int SCENE_MENU = 1;
// const int SCENE_START = 2;
enum {
	SCENE_MENU = 1,
	SCENE_START = 2,
	SCENE_LEVEL_2 = 3,
	SCENE_LEVEL_3 = 4,
	SCENE_SETTINGS = 5,
	SCENE_GAMEOVER = 6
};

/* basic data */
const int LEVEL_2_SCORE = 200;
const int LEVEL_3_SCORE = 400;

const float ENEMIES_ATTACK = 3;
const float BIGENEMY_ATTACK = 5;
const float FIRE_ATTACK = 1;
const float BIGFIRE_ATTACK = 3;

const float PLANE_SPEED = 5;
const float BULLETS_SPEED = 12;
const float ENEMIES_SPEED = 2;
const float FIRE_SPEED = 3;
const float BIGENEMY_SPEED = 1;
const float BIGFIRE_SPEED = 8;

#define START_LEVEL 1
#define START_SCORE 0
#define START_LIVES 3
#define MAX_HP 100
#define MAX_bigenemy_HP 100
int level;
float score;
int lives;
float HP;
float bigenemy_HP;
float defense;
//add fire
int add_f;
//add enemies
int add_e;
int choose_role = 1;

/* Input states */
// The active scene id.
int active_scene;
// Keyboard state, whether the key is down or not.
bool key_state[ALLEGRO_KEY_MAX];
// Mouse state, whether the key is down or not.
// 1 is for left, 2 is for right, 3 is for middle.
bool *mouse_state;
// Mouse position.
int mouse_x, mouse_y;

/* Variables for allegro basic routines. */
ALLEGRO_DISPLAY* game_display;
ALLEGRO_EVENT_QUEUE* game_event_queue;
ALLEGRO_TIMER* game_update_timer;

/* Shared resources*/
ALLEGRO_FONT* font_pirulen_32;
ALLEGRO_FONT* font_pirulen_24;
ALLEGRO_FONT* font_pirulen_20;
ALLEGRO_FONT* font_CoconutCookies_40;
ALLEGRO_FONT* font_28DaysLater_32;
ALLEGRO_FONT* font_regular_20;
ALLEGRO_BITMAP* img_button;
ALLEGRO_SAMPLE* button_sound;
ALLEGRO_SAMPLE_ID button_sound_id;

/* Menu Scene resources*/
ALLEGRO_BITMAP* main_img_background;
ALLEGRO_BITMAP* img_settings;
ALLEGRO_BITMAP* img_settings2;
ALLEGRO_SAMPLE* main_bgm;
ALLEGRO_SAMPLE_ID main_bgm_id;

/* Start Scene resources*/
// picture
ALLEGRO_BITMAP* start_img_background;
ALLEGRO_BITMAP* start_img_level_2;
ALLEGRO_BITMAP* start_img_level_3;
ALLEGRO_BITMAP* start_img_heart;
ALLEGRO_BITMAP* start_img_black;
ALLEGRO_BITMAP* start_img_black2;
ALLEGRO_BITMAP* start_img_hp;
ALLEGRO_BITMAP* start_img_plane_1;
ALLEGRO_BITMAP* start_img_plane_2;
ALLEGRO_BITMAP* start_img_plane_3;
ALLEGRO_BITMAP* start_img_enemy;
ALLEGRO_BITMAP* start_img_bigenemy;
ALLEGRO_BITMAP* img_bullet;
ALLEGRO_BITMAP* img_fire;
ALLEGRO_BITMAP* img_bigfire;
//music
ALLEGRO_SAMPLE* start_bgm;
ALLEGRO_SAMPLE_ID start_bgm_id;
ALLEGRO_SAMPLE* start_level_2_bgm;
ALLEGRO_SAMPLE_ID start_level_2_bgm_id;
ALLEGRO_SAMPLE* start_level_3_bgm;
ALLEGRO_SAMPLE_ID start_level_3_bgm_id;
ALLEGRO_SAMPLE* start_shoot_sound;
ALLEGRO_SAMPLE_ID start_shoot_sound_id;
ALLEGRO_SAMPLE* start_collide_sound;
ALLEGRO_SAMPLE_ID start_collide_sound_id;
ALLEGRO_SAMPLE* start_fire_sound;
ALLEGRO_SAMPLE_ID start_fire_sound_id;

/* Setting Scene resources*/
ALLEGRO_BITMAP* img_board;
ALLEGRO_BITMAP* img_square1;
ALLEGRO_BITMAP* img_square2;
ALLEGRO_BITMAP* setting_img_plane_1;
ALLEGRO_BITMAP* setting_img_plane_2;
ALLEGRO_BITMAP* setting_img_plane_3;
ALLEGRO_SAMPLE* setting_bgm;
ALLEGRO_SAMPLE_ID setting_bgm_id;

/* Game Over Scene resources*/
ALLEGRO_BITMAP* gameover_img_background;
ALLEGRO_SAMPLE* gameover_bgm;
ALLEGRO_SAMPLE_ID gameover_bgm_id;

/* movable object */
typedef struct {
	// The center coordinate of the image.
	float x, y;
	// The width and height of the object.
	float w, h;
	// The velocity in x, y axes.
	float vx, vy;
	// Should we draw this object on the screen.
	bool hidden;
	// The pointer to the object¡¦s image.
	ALLEGRO_BITMAP* img;
} MovableObject;

void draw_movable_object(MovableObject obj);

#define MAX_ENEMY 4
#define MAX_BULLET 5
#define MAX_FIRE 15
#define MAX_BIGFIRE 100
MovableObject plane;
MovableObject bullets[MAX_BULLET];
MovableObject enemies[MAX_ENEMY+50];
MovableObject fire[MAX_FIRE+500];
MovableObject bigenemy;
MovableObject bigfire[MAX_BIGFIRE];

//bullet shooting cool-down variables
float MAX_COOLDOWN;
//bullet last shoot
double last_shoot_timestamp_b;
//fire last shoot
double last_shoot_timestamp_f;
//bigfire last shoot
double last_shoot_timestamp_bf;
double last_bigenemy_die_timestamp;
//last shoot index in bigfire array
int last_bigfire = 0;

/* Declare function prototypes. */
// Initialize allegro5 library
void allegro5_init(void);
// Initialize variables and resources.
// Allows the game to perform any initialization it needs before
// starting to run.
void game_init(void);
// Process events inside the event queue using an infinity loop.
void game_start_event_loop(void);
// Run game logic such as updating the world, checking for collision,
// switching scenes and so on.
// This is called when the game should update its logic.
void game_update(void);
// Draw to display.
// This is called when the game should draw itself.
void game_draw(void);
// Release resources.
// Free the pointers we allocated.
void game_destroy(void);
// Function to change from one scene to another.
void game_change_scene(int next_scene);
// Load resized bitmap and check if failed.
ALLEGRO_BITMAP *load_bitmap_resized(const char *filename, int w, int h);
// Determines whether the point (px, py) is in rect (x, y, w, h).
bool pnt_in_rect(int px, int py, int x, int y, int w, int h);

/* Event callbacks. */
void on_key_down(int keycode);
void on_mouse_down(int btn, int x, int y);

/* Declare function prototypes for debugging. */
// Display error message and exit the program, used like 'printf'.
// Write formatted output to stdout and file from the format string.
// If the program crashes unexpectedly, you can inspect "log.txt" for
// further information.
void game_abort(const char* format, ...);
// Log events for later debugging, used like 'printf'.
// Write formatted output to stdout and file from the format string.
// You can inspect "log.txt" for logs in the last run.
void game_log(const char* format, ...);
// Log using va_list.
void game_vlog(const char* format, va_list arg);

int main(int argc, char** argv) {
	// Set random seed for better random outcome.
	srand(time(NULL));
	allegro5_init();
	game_log("Allegro5 initialized");
	game_log("Game begin");
	// Initialize game variables.
	game_init();
	game_log("Game initialized");
	// Draw the first frame.
	game_draw();
	game_log("Game start event loop");
	// This call blocks until the game is finished.
	game_start_event_loop();
	game_log("Game end");
	game_destroy();
	return 0;
}

void allegro5_init(void) {
	if (!al_init())
		game_abort("failed to initialize allegro");

	// Initialize add-ons.
	if (!al_init_primitives_addon())
		game_abort("failed to initialize primitives add-on");
	if (!al_init_font_addon())
		game_abort("failed to initialize font add-on");
	if (!al_init_ttf_addon())
		game_abort("failed to initialize ttf add-on");
	if (!al_init_image_addon())
		game_abort("failed to initialize image add-on");
	if (!al_install_audio())
		game_abort("failed to initialize audio add-on");
	if (!al_init_acodec_addon())
		game_abort("failed to initialize audio codec add-on");
	if (!al_reserve_samples(RESERVE_SAMPLES))
		game_abort("failed to reserve samples");
	if (!al_install_keyboard())
		game_abort("failed to install keyboard");
	if (!al_install_mouse())
		game_abort("failed to install mouse");
	// TODO: Initialize other addons such as video, ...

	// Setup game display.
	game_display = al_create_display(SCREEN_W, SCREEN_H);
	if (!game_display)
		game_abort("failed to create display");
	al_set_window_title(game_display, "I2P(I)_2019 Final Project <108062108>");

	// Setup update timer.
	game_update_timer = al_create_timer(1.0f / FPS);
	if (!game_update_timer)
		game_abort("failed to create timer");

	// Setup event queue.
	game_event_queue = al_create_event_queue();
	if (!game_event_queue)
		game_abort("failed to create event queue");

	// Malloc mouse buttons state according to button counts.
	const unsigned m_buttons = al_get_mouse_num_buttons();
	game_log("There are total %u supported mouse buttons", m_buttons);
	// mouse_state[0] will not be used.
	mouse_state = malloc((m_buttons + 1) * sizeof(bool));
	memset(mouse_state, false, (m_buttons + 1) * sizeof(bool));

	// Register display, timer, keyboard, mouse events to the event queue.
	al_register_event_source(game_event_queue, al_get_display_event_source(game_display));
	al_register_event_source(game_event_queue, al_get_timer_event_source(game_update_timer));
	al_register_event_source(game_event_queue, al_get_keyboard_event_source());
	al_register_event_source(game_event_queue, al_get_mouse_event_source());
	// TODO: Register other event sources such as timer, video, ...

	// Start the timer to update and draw the game.
	al_start_timer(game_update_timer);
}

void game_init(void) {
	/* Shared resources*/
	//font
	font_pirulen_32 = al_load_font("pirulen.ttf", 32, 0);
	if (!font_pirulen_32)
		game_abort("failed to load font: pirulen.ttf with size 32");

	font_pirulen_24 = al_load_font("pirulen.ttf", 24, 0);
	if (!font_pirulen_24)
		game_abort("failed to load font: pirulen.ttf with size 24");

    font_pirulen_20 = al_load_font("pirulen.ttf", 20, 0);
	if (!font_pirulen_20)
		game_abort("failed to load font: pirulen.ttf with size 16");

    font_CoconutCookies_40 = al_load_font("CoconutCookies.ttf", 40, 0);
    if (!font_CoconutCookies_40)
		game_abort("failed to load font: CoconutCookies.ttf with size 40");

    font_28DaysLater_32 = al_load_font("28 Days Later.ttf", 56, 0);
    if (!font_28DaysLater_32)
		game_abort("failed to load font: CoconutCookies.ttf with size 32");

    font_regular_20 = al_load_font("regular.ttf", 20, 0);
    if (!font_regular_20)
		game_abort("failed to load font: regualr.ttf with size 20");

    //object
    img_button = load_bitmap_resized("button.png", 200, 80);
    if (!img_button)
		game_abort("failed to load image: button.png");

    //music
    button_sound = al_load_sample("button.ogg");
	if (!button_sound)
		game_abort("failed to load audio: button.ogg");

	/* Menu Scene resources*/
	main_img_background = load_bitmap_resized("main-bg.jpg", SCREEN_W, SCREEN_H);
	if (!main_img_background)
		game_abort("failed to load image: main-bg.jpg");

	main_bgm = al_load_sample("boy.ogg");
	if (!main_bgm)
		game_abort("failed to load audio: boy.ogg");

	img_settings = al_load_bitmap("settings.png");
	if (!img_settings)
		game_abort("failed to load image: settings.png");

	img_settings2 = al_load_bitmap("settings2.png");
	if (!img_settings2)
		game_abort("failed to load image: settings2.png");

	/* Start Scene resources*/
    //background
	start_img_background = load_bitmap_resized("level_1.jpg", SCREEN_W, SCREEN_H);
	if (!start_img_background)
		game_abort("failed to load image: level_1.jpg");

    start_img_level_2 = load_bitmap_resized("level_2.jpg", SCREEN_W, SCREEN_H);
    if (!start_img_level_2)
		game_abort("failed to load image: level_2.jpg");

    start_img_level_3 = load_bitmap_resized("level_3.jpg", SCREEN_W, SCREEN_H);
    if (!start_img_level_3)
		game_abort("failed to load image: level_3.jpg");

    //object
    start_img_black = load_bitmap_resized("black.jpg", 306, 21);
    if (!start_img_black)
		game_abort("failed to load image: black.jpg");

    start_img_black2 = load_bitmap_resized("black.jpg", 102, 12);
    if (!start_img_black2)
		game_abort("failed to load image: black.jpg");

    start_img_heart = load_bitmap_resized("heart.png", 30, 30);
    if (!start_img_black)
		game_abort("failed to load image: heart.png");

	start_img_plane_1 = load_bitmap_resized("plane1.png", 80, 80);
	if (!start_img_plane_1)
		game_abort("failed to load image: plane.png");

    start_img_plane_2 = load_bitmap_resized("plane2.png", 80, 80);
	if (!start_img_plane_2)
		game_abort("failed to load image: plane.png");

    start_img_plane_3 = load_bitmap_resized("plane3.png", 80, 80);
	if (!start_img_plane_3)
		game_abort("failed to load image: plane.png");

	start_img_enemy = load_bitmap_resized("smallfighter.png", 60, 40);
	if (!start_img_enemy)
		game_abort("failed to load image: smallfighter.png");

    start_img_bigenemy = load_bitmap_resized("bad.png", 120, 110);
	if (!start_img_bigenemy)
		game_abort("failed to load image: bad.png");

	img_bullet = al_load_bitmap("image12.png");
	if (!img_bullet)
		game_abort("failed to load image: image12.png");

    img_fire = load_bitmap_resized("fire.png", 30, 30);
	if (!img_fire)
		game_abort("failed to load image: fire.png");

    img_bigfire = load_bitmap_resized("bigfire.png", 30, 30);
	if (!img_bigfire)
		game_abort("failed to load image: bigfire.png");

    //music
    start_bgm = al_load_sample("mythica.ogg");
	if (!start_bgm)
		game_abort("failed to load audio: mythica.ogg");

    start_level_2_bgm = al_load_sample("good music.ogg");
	if (!start_level_2_bgm)
		game_abort("failed to load audio: good music.ogg");

    start_level_3_bgm = al_load_sample("space.ogg");
	if (!start_level_3_bgm)
		game_abort("failed to load audio: space.ogg");

    start_shoot_sound = al_load_sample("shoot.ogg");
	if (!start_shoot_sound)
		game_abort("failed to load audio: shoot.ogg");

    start_collide_sound = al_load_sample("bomb.ogg");
	if (!start_collide_sound)
		game_abort("failed to load audio: bomb.ogg");

    start_fire_sound = al_load_sample("damage6.ogg");
	if (!start_fire_sound)
		game_abort("failed to load audio: damage6.ogg");

    /* Setting Scene resources*/
    img_board = load_bitmap_resized("board.jpg", 700, 380);
    if (!img_board)
		game_abort("failed to load image: board.jpg");

    img_square1 = load_bitmap_resized("square1.png", 170, 170);
	if (!img_square1)
		game_abort("failed to load image: square1.png");

    img_square2 = load_bitmap_resized("square2.png", 170, 170);
	if (!img_square2)
		game_abort("failed to load image: square2.png");

    setting_img_plane_1 = load_bitmap_resized("plane1.png", 150, 150);
	if (!setting_img_plane_1)
		game_abort("failed to load image: plane1.png");

    setting_img_plane_2 = load_bitmap_resized("plane2.png", 150, 150);
	if (!setting_img_plane_2)
		game_abort("failed to load image: plane2.png");

    setting_img_plane_3 = load_bitmap_resized("plane3.png", 150, 150);
	if (!setting_img_plane_3)
		game_abort("failed to load image: plane3.png");

    setting_bgm = al_load_sample("S31-Night Prowler.ogg");
	if (!start_bgm)
		game_abort("failed to load audio: S31-Night Prowler.ogg");

    /* Gameover Scene resources*/
    gameover_img_background = load_bitmap_resized("gameover.png", SCREEN_W, SCREEN_H);
    if (!gameover_img_background)
		game_abort("failed to load image: gameover.png");

    gameover_bgm = al_load_sample("gameover.ogg");
	if (!gameover_bgm)
		game_abort("failed to load audio: gameover.ogg");

	// Change to first scene.
	game_change_scene(SCENE_MENU);
}

void game_start_event_loop(void) {
	bool done = false;
	ALLEGRO_EVENT event;
	int redraws = 0;
	while (!done) {
		al_wait_for_event(game_event_queue, &event);
		if (event.type == ALLEGRO_EVENT_DISPLAY_CLOSE) {
			// Event for clicking the window close button.
			game_log("Window close button clicked");
			done = true;
		} else if (event.type == ALLEGRO_EVENT_TIMER) {
			// Event for redrawing the display.
			if (event.timer.source == game_update_timer)
				// The redraw timer has ticked.
				redraws++;
		} else if (event.type == ALLEGRO_EVENT_KEY_DOWN) {
			// Event for keyboard key down.
			game_log("Key with keycode %d down", event.keyboard.keycode);
			key_state[event.keyboard.keycode] = true;
			on_key_down(event.keyboard.keycode);
		} else if (event.type == ALLEGRO_EVENT_KEY_UP) {
			// Event for keyboard key up.
			game_log("Key with keycode %d up", event.keyboard.keycode);
			key_state[event.keyboard.keycode] = false;
		} else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_DOWN) {
			// Event for mouse key down.
			game_log("Mouse button %d down at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
			mouse_state[event.mouse.button] = true;
			on_mouse_down(event.mouse.button, event.mouse.x, event.mouse.y);
		} else if (event.type == ALLEGRO_EVENT_MOUSE_BUTTON_UP) {
			// Event for mouse key up.
			game_log("Mouse button %d up at (%d, %d)", event.mouse.button, event.mouse.x, event.mouse.y);
			mouse_state[event.mouse.button] = false;
		} else if (event.type == ALLEGRO_EVENT_MOUSE_AXES) {
			if (event.mouse.dx != 0 || event.mouse.dy != 0) {
				// Event for mouse move.
				game_log("Mouse move to (%d, %d)", event.mouse.x, event.mouse.y);
				mouse_x = event.mouse.x;
				mouse_y = event.mouse.y;
			} else if (event.mouse.dz != 0) {
				// Event for mouse scroll.
				game_log("Mouse scroll at (%d, %d) with delta %d", event.mouse.x, event.mouse.y, event.mouse.dz);
			}
		}
		// TODO: Process more events and call callbacks by adding more
		// entries inside Scene.

		// Redraw
		if (redraws > 0 && al_is_event_queue_empty(game_event_queue)) {
			// if (redraws > 1)
			// 	game_log("%d frame(s) dropped", redraws - 1);
			// Update and draw the next frame.
			game_update();
			game_draw();
			redraws = 0;
		}
	}
}

void game_update(void) {
	double now = al_get_time();
	//level 1,2,3
	if (active_scene == SCENE_START || active_scene == SCENE_LEVEL_2 || active_scene == SCENE_LEVEL_3) {
		/* bigenemy and bigfire */
		if(active_scene == SCENE_LEVEL_3) {
            //bigenemy move
            if (!bigenemy.hidden) {
                if (bigenemy.x - bigenemy.w/2 < 0) {
                    bigenemy.x = bigenemy.w/2;
                    bigenemy.vx *= -1;
                }
                else if (bigenemy.x + bigenemy.w/2 > SCREEN_W) {
                    bigenemy.x = SCREEN_W - bigenemy.w/2;
                    bigenemy.vx *= -1;
                }
                if (bigenemy.y - bigenemy.h/2 < 0) {
                    bigenemy.y = bigenemy.h/2;
                    bigenemy.vy *= -1;
                }
                else if (bigenemy.y + bigenemy.h/2 > SCREEN_H - 70) {
                    bigenemy.y = SCREEN_H - 70 - bigenemy.h/2;
                    bigenemy.vy *= -1;
                }
                bigenemy.x += bigenemy.vx;
                bigenemy.y += bigenemy.vy;
            }

            //bigfire move
            for (i = 0; i < MAX_BIGFIRE; i++) {
                if (!bigfire[i].hidden) {
                    if (bigfire[i].x - bigfire[i].w/2 < 0)
                        bigfire[i].hidden = true;
                    else if (bigfire[i].x + bigfire[i].w/2 > SCREEN_W)
                        bigfire[i].hidden = true;
                    if (bigfire[i].y - bigfire[i].h/2 < 0)
                        bigfire[i].hidden = true;
                    else if (bigfire[i].y + bigfire[i].h/2 > SCREEN_H)
                        bigfire[i].hidden = true;
                    bigfire[i].x += bigfire[i].vx;
                    bigfire[i].y += bigfire[i].vy;
                }
            }

            //bigenemy collide plane
            if(pnt_in_rect(bigenemy.x, bigenemy.y, plane.x-plane.w/2, plane.y-plane.h/2, plane.w, plane.h) ||
               pnt_in_rect(plane.x, plane.y, bigenemy.x-bigenemy.w/2, bigenemy.y-bigenemy.h/2, bigenemy.w, bigenemy.h))
            {
                HP -= BIGENEMY_ATTACK*(2.5-lives/2)*defense;
                bigenemy_HP -= 2;
                plane.x  -= plane.vx*50;
                plane.y  -= plane.vy*50;
                al_play_sample(start_collide_sound, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &start_collide_sound_id);
            }

            //bigfire shoot (5 bigfire)
            if (now - last_shoot_timestamp_bf >= 1.0f && !bigenemy.hidden) {
                if (last_bigfire+5 > MAX_BIGFIRE-1)
                    last_bigfire = 0;
                last_shoot_timestamp_bf = now;
                for(i = last_bigfire; i < last_bigfire+5; i++) {
                    bigfire[i].hidden = false;
                    bigfire[i].x = bigenemy.x;
                    bigfire[i].y = bigenemy.y + bigenemy.h/2;
                }
                last_bigfire += 5;
            }

            //bigfire attack to plane -> HP reduce
            for(i = 0; i < MAX_BIGFIRE; i++) {
                if(pnt_in_rect(bigfire[i].x, bigfire[i].y, plane.x-plane.w/2, plane.y-plane.h/2, plane.w, plane.h)) {
                    HP -= BIGFIRE_ATTACK*(2.5-lives/2);
                    bigfire[i].y = SCREEN_H*2; //bigfire disappear
                    al_play_sample(start_fire_sound, 0.8, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &start_fire_sound_id);
                }
            }

            //bullets attack to bigenemy
            for(i = 0; i < MAX_BULLET; i++) {
                if(pnt_in_rect(bullets[i].x, bullets[i].y, bigenemy.x-bigenemy.w/2, bigenemy.y-bigenemy.h/2, bigenemy.w, bigenemy.h)) {
                    bigenemy_HP -= 2;
                    bullets[i].y = -10; //bullet disappear
                    al_play_sample(start_shoot_sound, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &start_shoot_sound_id);
                }
            }

            //bigenemy die
            if (bigenemy_HP <= 0 && (!bigenemy.hidden)) {
                score += 500;
                bigenemy.y = SCREEN_H + 100; //bigenemy disappear
                bigenemy.hidden = true;
                last_bigenemy_die_timestamp = now;
            }

            //bigenemy return
            if (bigenemy.hidden && now-last_bigenemy_die_timestamp > 5.0f) {
                bigenemy.hidden = false;
                bigenemy.x = SCREEN_W/2;
                bigenemy.y = SCREEN_H/2;
                bigenemy_HP = MAX_bigenemy_HP;
            }
        }

        /* plane move */
		plane.vx = plane.vy = 0;
		//plane move - keyboard
		if (key_state[ALLEGRO_KEY_UP] || key_state[ALLEGRO_KEY_W])
			plane.vy -= 1;
		if (key_state[ALLEGRO_KEY_DOWN] || key_state[ALLEGRO_KEY_S])
			plane.vy += 1;
		if (key_state[ALLEGRO_KEY_LEFT] || key_state[ALLEGRO_KEY_A])
			plane.vx -= 1;
		if (key_state[ALLEGRO_KEY_RIGHT] || key_state[ALLEGRO_KEY_D])
			plane.vx += 1;

        //plane move - mouse
        if(mouse_state[2]) {
            int dx = mouse_x-plane.x;
            int dy = mouse_y-plane.y;
            if (dx!=0)
                plane.vx = 1.41*dx/sqrt(dx*dx+dy*dy);
            if (dy!=0)
                plane.vy = 1.41*dy/sqrt(dx*dx+dy*dy);
        }
		// 0.71 is (1/sqrt(2)).
		plane.y += plane.vy * PLANE_SPEED * (plane.vx ? 0.71f : 1);
		plane.x += plane.vx * PLANE_SPEED * (plane.vy ? 0.71f : 1);

		// plane in the frame
		if (plane.x - plane.w/2 < 0)
			plane.x = plane.w/2;
		else if (plane.x + plane.w/2 > SCREEN_W)
			plane.x = SCREEN_W - plane.w/2;
		if (plane.y - plane.h/2 < 0)
			plane.y = plane.h/2;
		else if (plane.y + plane.h/2 > SCREEN_H - 70)
			plane.y = SCREEN_H - 70 - plane.h/2;

        // bullets move
		for (i = 0; i < MAX_BULLET; i++) {
			if (bullets[i].hidden)
				continue;
			bullets[i].y += bullets[i].vy;
			if (bullets[i].y - bullets[i].h/2 < 0)
                bullets[i].hidden = true;
		}

        // enemies move
        for(i = 0; i < MAX_ENEMY + add_e; i++) {
            if (enemies[i].hidden) {
                enemies[i].hidden = false;
                enemies[i].x = enemies[i].w / 2 + (float)rand() / RAND_MAX * (SCREEN_W - enemies[i].w);
                enemies[i].y = 0;
            }
            enemies[i].y += enemies[i].vy;
            if (enemies[i].y + enemies[i].h/2 > SCREEN_H)
                enemies[i].hidden = true;
        }

        // fire move
		for (i = 0; i + 1 < MAX_FIRE + add_f; i+=2) {
			if (!fire[i].hidden) {
                fire[i].x += fire[i].vx;
                fire[i].y += fire[i].vy;
                if (fire[i].x - fire[i].w/2 < 0)
                    fire[i].hidden = true;
                else if (fire[i].x + fire[i].w/2 > SCREEN_W)
                    fire[i].hidden = true;
                if (fire[i].y + fire[i].h/2 > SCREEN_H)
                    fire[i].hidden = true;
			}
			if (!fire[i+1].hidden) {
                fire[i+1].x -= fire[i+1].vx;
                fire[i+1].y += fire[i+1].vy;
                if (fire[i+1].x - fire[i+1].w/2 < 0)
                    fire[i+1].hidden = true;
                else if (fire[i+1].x + fire[i+1].w/2 > SCREEN_W)
                    fire[i+1].hidden = true;
                if (fire[i+1].y + fire[i+1].h/2 > SCREEN_H)
                    fire[i+1].hidden = true;
			}
		}

        //plane collide enemy
        for(i = 0; i < MAX_ENEMY + add_e; i++)
            if(pnt_in_rect(enemies[i].x, enemies[i].y, plane.x-plane.w/2, plane.y-plane.h/2, plane.w, plane.h) ||
               pnt_in_rect(plane.x, plane.y, enemies[i].x-enemies[i].w/2, enemies[i].y-enemies[i].h/2, enemies[i].w, enemies[i].h))
            {
                HP -= ENEMIES_ATTACK*(2.5-lives/2)*defense;
                score += 10;
                enemies[i].y = SCREEN_H*2; //enemy disappear
                al_play_sample(start_collide_sound, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &start_collide_sound_id);
            }

        //bullet shoot
		if ((key_state[ALLEGRO_KEY_SPACE] || mouse_state[1]) && now - last_shoot_timestamp_b >= MAX_COOLDOWN) {
		    for (i = 0; i < MAX_BULLET; i++) {
		        if (bullets[i].hidden)
		            break;
		    }
		    if (i < MAX_BULLET) {
                last_shoot_timestamp_b = now;
		        bullets[i].hidden = false;
		        bullets[i].x = plane.x;
		        bullets[i].y = plane.y - plane.h/2;
		    }
		}

		//fire shoot
		if (now - last_shoot_timestamp_f >= 0.2f) {
		    for (i = 0; i + 1 < MAX_FIRE + add_f; i+=2) {
		        if (fire[i].hidden && fire[i+1].hidden)
		            break;
		    }
		    if (i + 1 < MAX_FIRE + add_f) {
                last_shoot_timestamp_f = now;
                fire[i].hidden = false;
                fire[i+1].hidden = false;
                int r = rand()%(MAX_ENEMY + add_e);
                fire[i].x = enemies[r].x;
                fire[i].y = enemies[r].y + enemies[r].h/2;
                fire[i+1].x = enemies[r].x;
                fire[i+1].y = enemies[r].y + enemies[r].h/2;
		    }
		}


		//bullets attack to enemies
        for(i = 0; i < MAX_BULLET; i++) {
            for(j = 0; j < MAX_ENEMY + add_e; j++)
                if(pnt_in_rect(bullets[i].x, bullets[i].y, enemies[j].x-enemies[j].w/2, enemies[j].y-enemies[j].h/2, enemies[j].w, enemies[j].h)) {
                    score += 10;
                    enemies[j].y = SCREEN_H*2; //enemy disappear
                    bullets[i].y = -10; //bullet disappear
                    al_play_sample(start_shoot_sound, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &start_shoot_sound_id);
                }
		}

		//fire attack to plane
		for(i = 0; i < MAX_FIRE + add_f; i++)
            if(pnt_in_rect(fire[i].x, fire[i].y, plane.x-plane.w/2, plane.y-plane.h/2, plane.w, plane.h)) {
                HP -= FIRE_ATTACK*(2.5-lives/2)*defense;
                fire[i].y = SCREEN_H*2; //fire disappear
                al_play_sample(start_fire_sound, 0.8, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &start_fire_sound_id);
            }

        //lives reduce
        if(HP<=0) {
            HP = MAX_HP;
            lives--;
        }

        //gameover
        if(lives<=0)
            game_change_scene(SCENE_GAMEOVER);

        //level 1 to 2
        if(score >= LEVEL_2_SCORE && score < LEVEL_3_SCORE && active_scene == SCENE_START) {
            game_change_scene(SCENE_LEVEL_2);
            level = 2;
        }

        //level 2 to 3
        if(score >= LEVEL_3_SCORE && active_scene == SCENE_LEVEL_2) {
            game_change_scene(SCENE_LEVEL_3);
            level = 3;
        }
	}
}

void game_draw(void) {
	/* menu */
	if (active_scene == SCENE_MENU) {
		//backgroung
		al_draw_bitmap(main_img_background, 0, 0, 0);

		//text
		al_draw_text(font_28DaysLater_32, al_map_rgb(255, 255, 255), SCREEN_W / 2, 30, ALLEGRO_ALIGN_CENTER, "Space Shooter");
		al_draw_text(font_pirulen_20, al_map_rgb(255, 255, 255), 20, SCREEN_H - 50, 0, "Press enter key to start");

		//setting button
		if (pnt_in_rect(mouse_x, mouse_y, SCREEN_W - 48, 10, 38, 38))
			al_draw_bitmap(img_settings2, SCREEN_W - 48, 10, 0);
		else
			al_draw_bitmap(img_settings, SCREEN_W-48, 10, 0);

    }

    /* level 1,2,3 */
    else if (active_scene == SCENE_START || active_scene == SCENE_LEVEL_2 || active_scene == SCENE_LEVEL_3) {
		//background
		if(active_scene == SCENE_START)
            al_draw_bitmap(start_img_background, 0, 0, 0);
        else if(active_scene == SCENE_LEVEL_2)
            al_draw_bitmap(start_img_level_2, 0, 0, 0);
        else if(active_scene == SCENE_LEVEL_3)
            al_draw_bitmap(start_img_level_3, 0, 0, 0);

        //object
        al_draw_bitmap(start_img_black, 247, 567, 0);
        start_img_hp = load_bitmap_resized("hp.jpg", 3*HP, 15);
        al_draw_bitmap(start_img_hp, 250, 570, 0);
        if (lives == 3) {
            al_draw_bitmap(start_img_heart, 350, 530, 0);
            al_draw_bitmap(start_img_heart, 400, 530, 0);
            al_draw_bitmap(start_img_heart, 450, 530, 0);
        }
        else if (lives == 2) {
            al_draw_bitmap(start_img_heart, 350, 530, 0);
            al_draw_bitmap(start_img_heart, 400, 530, 0);
        }
        else if (lives == 1) {
            al_draw_bitmap(start_img_heart, 350, 530, 0);
        }

		//text
		al_draw_textf(font_pirulen_24, al_map_rgb(0, 0, 0), 20, 20, 0, "Level: %d", level);
		al_draw_textf(font_pirulen_24, al_map_rgb(0, 0, 0), 20, 50, 0, "Score: %.0f", score);
		al_draw_text(font_pirulen_20, al_map_rgb(0, 0, 0), 250, 535, 0, "Lives: " );
		al_draw_textf(font_pirulen_20, al_map_rgb(0, 0, 0), 90, 565, 0, "HP: %.0f/%d", HP, MAX_HP);

        //movable object - basic
        draw_movable_object(plane);
		for (i = 0; i < MAX_BULLET; i++)
			draw_movable_object(bullets[i]);
        for (i = 0; i < MAX_FIRE + add_f; i++)
			draw_movable_object(fire[i]);
		for (i = 0; i < MAX_ENEMY + add_e; i++)
			draw_movable_object(enemies[i]);

        //movable object - level 3
        if (active_scene == SCENE_LEVEL_3) {
            draw_movable_object(bigenemy);
            for (i = 0; i < MAX_BIGFIRE; i++)
                draw_movable_object(bigfire[i]);
            if(!bigenemy.hidden) {
                al_draw_bitmap(start_img_black2, bigenemy.x-bigenemy.w/2-1, bigenemy.y-71, 0);
                start_img_hp = load_bitmap_resized("hp.jpg", bigenemy_HP, 10);
                al_draw_bitmap(start_img_hp, bigenemy.x-bigenemy.w/2, bigenemy.y-70, 0);
            }
        }
	}

	/* setting */
	else if (active_scene == SCENE_SETTINGS) {
		//background
		al_clear_to_color(al_map_rgb(200, 255, 255));

		//object
		al_draw_bitmap(img_board, 50, 70, 0);
		al_draw_bitmap(setting_img_plane_1, 100, 250, 0);
		al_draw_bitmap(setting_img_plane_2, 325, 250, 0);
		al_draw_bitmap(setting_img_plane_3, 550, 250, 0);
		al_draw_bitmap(img_button, 150, 480, 0);
		al_draw_bitmap(img_button, 450, 480, 0);

		//text
		al_draw_text(font_pirulen_32, al_map_rgb(0, 0, 0), SCREEN_W / 2, 20, ALLEGRO_ALIGN_CENTER, "Setting");
		al_draw_text(font_CoconutCookies_40, al_map_rgb(0, 0, 0), SCREEN_W / 2, 110, ALLEGRO_ALIGN_CENTER, "Choose Role");
		al_draw_text(font_pirulen_24, al_map_rgb(0, 0, 0), 250, 508, ALLEGRO_ALIGN_CENTER, "Menu");
		al_draw_text(font_pirulen_24, al_map_rgb(0, 0, 0), 550, 508, ALLEGRO_ALIGN_CENTER, "Start");
		al_draw_text(font_regular_20, al_map_rgb(105, 90, 205), 180, 180, ALLEGRO_ALIGN_CENTER, "Cool down: short");
		al_draw_text(font_regular_20, al_map_rgb(105, 90, 205), 180, 210, ALLEGRO_ALIGN_CENTER, "defense: low");
		al_draw_text(font_regular_20, al_map_rgb(105, 90, 205), 400, 180, ALLEGRO_ALIGN_CENTER, "Cool down: medium");
		al_draw_text(font_regular_20, al_map_rgb(105, 90, 205), 400, 210, ALLEGRO_ALIGN_CENTER, "defense: medium");
		al_draw_text(font_regular_20, al_map_rgb(105, 90, 205), 630, 180, ALLEGRO_ALIGN_CENTER, "Cool down: long");
		al_draw_text(font_regular_20, al_map_rgb(105, 90, 205), 630, 210, ALLEGRO_ALIGN_CENTER, "defense: high");

		//choose role
        if (pnt_in_rect(mouse_x, mouse_y, 150, 250, 150, 150))
			al_draw_bitmap(img_square1, 90, 240, 0);
        else if (pnt_in_rect(mouse_x, mouse_y, 325, 250, 150, 150))
			al_draw_bitmap(img_square1, 315, 240, 0);
        else if (pnt_in_rect(mouse_x, mouse_y, 500, 250, 150, 150))
			al_draw_bitmap(img_square1, 540, 240, 0);
        if (choose_role==1)
            al_draw_bitmap(img_square2, 90, 240, 0);
        else if (choose_role==2)
            al_draw_bitmap(img_square2, 315, 240, 0);
        else if (choose_role==3)
            al_draw_bitmap(img_square2, 540, 240, 0);

	}

	/* gameover */
	else if (active_scene == SCENE_GAMEOVER) {
		//background
		al_draw_bitmap(gameover_img_background, 0, 0, 0);

        //object
        al_draw_bitmap(img_button, 150, 480, 0);
		al_draw_bitmap(img_button, 450, 480, 0);

		//text
		al_draw_textf(font_pirulen_24, al_map_rgb(255, 255, 255), 20, 20, 0, "Level: %d", level);
		al_draw_textf(font_pirulen_24, al_map_rgb(255, 255, 255), 20, 50, 0, "Score: %.0f", score);
		al_draw_text(font_pirulen_24, al_map_rgb(0, 0, 0), 250, 508, ALLEGRO_ALIGN_CENTER, "Menu");
		al_draw_text(font_pirulen_24, al_map_rgb(0, 0, 0), 550, 508, ALLEGRO_ALIGN_CENTER, "Restart");
	}

	al_flip_display();
}

void game_destroy(void) {
	// Destroy everything you have created.
	// Free the memories allocated by malloc or allegro functions.

	/* shared resources */
	al_destroy_font(font_pirulen_32);
	al_destroy_font(font_pirulen_24);
	al_destroy_font(font_pirulen_20);
	al_destroy_font(font_CoconutCookies_40);
	al_destroy_font(font_28DaysLater_32);
	al_destroy_font(font_regular_20);
	al_destroy_sample(button_sound);

	/* Menu Scene resources */
	al_destroy_bitmap(main_img_background);
	al_destroy_sample(main_bgm);
	al_destroy_bitmap(img_settings);
	al_destroy_bitmap(img_settings2);

	/* Start Scene resources*/
	al_destroy_bitmap(start_img_background);
	al_destroy_bitmap(start_img_level_2);
	al_destroy_bitmap(start_img_level_3);
	al_destroy_bitmap(start_img_heart);
	al_destroy_bitmap(start_img_black);
	al_destroy_bitmap(start_img_hp);
	al_destroy_bitmap(start_img_plane_1);
	al_destroy_bitmap(start_img_plane_2);
	al_destroy_bitmap(start_img_plane_3);
	al_destroy_bitmap(start_img_enemy);
	al_destroy_bitmap(start_img_bigenemy);
    al_destroy_bitmap(img_bigfire);
	al_destroy_bitmap(img_bullet);
	al_destroy_bitmap(img_fire);
	al_destroy_sample(start_bgm);
	al_destroy_sample(start_level_2_bgm);
	al_destroy_sample(start_level_3_bgm);
	al_destroy_sample(start_shoot_sound);
	al_destroy_sample(start_collide_sound);
    al_destroy_sample(start_fire_sound);

	/* Setting Scene resources*/
	al_destroy_bitmap(img_board);
	al_destroy_bitmap(setting_img_plane_1);
	al_destroy_bitmap(setting_img_plane_2);
	al_destroy_bitmap(setting_img_plane_3);
	al_destroy_bitmap(img_square1);
	al_destroy_bitmap(img_square2);
	al_destroy_bitmap(img_button);
	al_destroy_sample(setting_bgm);

	/* Gameover Scene resources*/
	al_destroy_bitmap(gameover_img_background);

	/* others */
	al_destroy_timer(game_update_timer);
	al_destroy_event_queue(game_event_queue);
	al_destroy_display(game_display);
	free(mouse_state);
}

void game_change_scene(int next_scene) {
	game_log("Change scene from %d to %d", active_scene, next_scene);

	// Destroy resources initialized when creating scene.
	if (active_scene == SCENE_MENU) {
		al_stop_sample(&main_bgm_id);
		game_log("stop audio (bgm)");
	}
	else if (active_scene == SCENE_START) {
		al_stop_sample(&start_bgm_id);
		game_log("stop audio (bgm)");
	}
	else if (active_scene == SCENE_LEVEL_2) {
		al_stop_sample(&start_level_2_bgm_id);
		game_log("stop audio (bgm)");
	}
	else if (active_scene == SCENE_LEVEL_3) {
		al_stop_sample(&start_level_3_bgm_id);
		game_log("stop audio (bgm)");
	}
	else if (active_scene == SCENE_SETTINGS) {
		al_stop_sample(&setting_bgm_id);
		game_log("stop audio (bgm)");
	}
	else if (active_scene == SCENE_GAMEOVER) {
		al_stop_sample(&gameover_bgm_id);
		game_log("stop audio (bgm)");
	}

	active_scene = next_scene;

	// Allocate resources before entering scene.
	/* menu */
	if (active_scene == SCENE_MENU) {
		if (!al_play_sample(main_bgm, 1.2, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &main_bgm_id))
			game_abort("failed to play audio (bgm)");
	}

	/* level 1,2,3 */
	else if (active_scene == SCENE_START || active_scene == SCENE_LEVEL_2 || active_scene == SCENE_LEVEL_3) {
		if(active_scene == SCENE_START) {
            add_e = 0;
            add_f = 0;
            level = START_LEVEL;
            lives = START_LIVES;
            HP = MAX_HP;
            score = START_SCORE;
            bigenemy_HP = MAX_bigenemy_HP;
            if (!al_play_sample(start_bgm, 2, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &start_bgm_id))
                game_abort("failed to play audio (bgm)");
		}
		if(active_scene == SCENE_LEVEL_2) {
            add_e = 2;
            add_f = 10;
            if (!al_play_sample(start_level_2_bgm, 2, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &start_level_2_bgm_id))
                game_abort("failed to play audio (bgm)");
		}
		if(active_scene == SCENE_LEVEL_3) {
            add_e = 3;
            add_f = 15                                                                                                                        ;
            if (!al_play_sample(start_level_3_bgm, 2, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &start_level_3_bgm_id))
                game_abort("failed to play audio (bgm)");

            //bigenemy
            bigenemy.img = start_img_bigenemy;
			bigenemy.w = al_get_bitmap_width(start_img_bigenemy);
			bigenemy.h = al_get_bitmap_height(start_img_bigenemy);
			bigenemy.x = SCREEN_W/2;
			bigenemy.y = SCREEN_H/2;
			bigenemy.vx = BIGENEMY_SPEED;
            bigenemy.vy = BIGENEMY_SPEED;
            bigenemy.hidden = false;

            //bigfire
            for (i = 0; i < MAX_BIGFIRE; i++) {
                bigfire[i].img = img_bigfire;
                bigfire[i].w = al_get_bitmap_width(img_bigfire);
                bigfire[i].h = al_get_bitmap_height(img_bigfire);
                bigfire[i].hidden = true;
                if(i%5==0) {
                    bigfire[i].vx = 1;
                    bigfire[i].vy = 0;
                }
                else if(i%5==1) {
                    bigfire[i].vx = 0.71;
                    bigfire[i].vy = 0.71;
                }
                else if(i%5==2) {
                    bigfire[i].vx = 0;
                    bigfire[i].vy = 1;
                }
                else if(i%5==3) {
                    bigfire[i].vx = -0.71;
                    bigfire[i].vy = 0.71;
                }
                else if(i%5==4) {
                    bigfire[i].vx = -1;
                    bigfire[i].vy = 0;
                }
                bigfire[i].vx *= BIGFIRE_SPEED;
                bigfire[i].vy *= BIGFIRE_SPEED;
            }
		}

        // plane
		if (choose_role==1) {
            plane.img = start_img_plane_1;
            MAX_COOLDOWN = 0.2f;
            defense = 1;
		}
        else if (choose_role==2) {
            plane.img = start_img_plane_2;
            MAX_COOLDOWN = 0.25f;
            defense = 0.7;
        }
        else if (choose_role==3) {
            plane.img = start_img_plane_3;
            MAX_COOLDOWN = 0.3f;
            defense = 0.4;
        }
		plane.w = al_get_bitmap_width(plane.img);
        plane.h = al_get_bitmap_height(plane.img);
		plane.x = 400;
		plane.y = 500;

        // enemies
		for (i = 0; i < MAX_ENEMY + add_e; i++) {
			enemies[i].img = start_img_enemy;
			enemies[i].w = al_get_bitmap_width(start_img_enemy);
			enemies[i].h = al_get_bitmap_height(start_img_enemy);
			enemies[i].vx = 0;
			enemies[i].vy = ENEMIES_SPEED;
			enemies[i].hidden = true;
		}

        // bullets
		for (i = 0; i < MAX_BULLET; i++) {
			bullets[i].img = img_bullet;
			bullets[i].w = al_get_bitmap_width(img_bullet);
			bullets[i].h = al_get_bitmap_height(img_bullet);
			bullets[i].x = plane.x;
            bullets[i].y = plane.y - plane.h/2;
			bullets[i].vx = 0;
			bullets[i].vy = -1*BULLETS_SPEED;
			bullets[i].hidden = true;
		}

        // fire
		for (i = 0; i < MAX_FIRE + add_f; i++) {
			fire[i].img = img_fire;
			fire[i].w = al_get_bitmap_width(img_fire);
			fire[i].h = al_get_bitmap_height(img_fire);
			fire[i].vx = 1*FIRE_SPEED;
			fire[i].vy = 3*FIRE_SPEED;
			fire[i].hidden = true;
		}
	}

	/* setting */
	else if (active_scene == SCENE_SETTINGS) {
        if (!al_play_sample(setting_bgm, 2, 0.0, 1.0, ALLEGRO_PLAYMODE_LOOP, &setting_bgm_id))
			game_abort("failed to play audio (bgm)");
	}

	/* gameover */
	else if (active_scene == SCENE_GAMEOVER) {
        if (!al_play_sample(gameover_bgm, 1, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &gameover_bgm_id))
			game_abort("failed to play audio (bgm)");
	}
}

void on_key_down(int keycode) {
    /* menu */
	if (active_scene == SCENE_MENU) {
		//start
		if (keycode == ALLEGRO_KEY_ENTER)
			game_change_scene(SCENE_START);
	}

	/* setting */
	if (active_scene == SCENE_SETTINGS) {
        //choose role
        if (choose_role == 2 && keycode == ALLEGRO_KEY_LEFT)
            choose_role = 1;
        else if ((choose_role == 1 && keycode == ALLEGRO_KEY_RIGHT) || (choose_role == 3 && keycode == ALLEGRO_KEY_LEFT))
            choose_role = 2;
        else if (choose_role == 2 && keycode == ALLEGRO_KEY_RIGHT)
            choose_role = 3;
	}
}

void on_mouse_down(int btn, int x, int y) {
    /* menu */
	if (active_scene == SCENE_MENU) {
		if (btn == true) {
			//setting
			if (pnt_in_rect(x, y, SCREEN_W - 48, 10, 38, 38)) {
				al_play_sample(button_sound, 1.2, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &button_sound_id);
				game_change_scene(SCENE_SETTINGS);
			}
		}
	}

	/* setting */
	else if (active_scene == SCENE_SETTINGS) {
		if (btn == true) {
            //choose role
            if (pnt_in_rect(x, y, 100, 250, 150, 150))
                choose_role = 1;
            else if (pnt_in_rect(x, y, 325, 250, 150, 150))
                choose_role = 2;
            else if (pnt_in_rect(x, y, 550, 250, 150, 150))
                choose_role = 3;

            //button - menu or start
            if (pnt_in_rect(x, y, 150, 480, 200, 80)) {
                game_change_scene(SCENE_MENU);
                al_play_sample(button_sound, 1.2, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &button_sound_id);
            }
            else if (pnt_in_rect(x, y, 450, 480, 200, 80)) {
                game_change_scene(SCENE_START);
                al_play_sample(button_sound, 1.2, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &button_sound_id);
            }
		}
	}

	/* gameover */
	else if (active_scene == SCENE_GAMEOVER) {
		if (btn == true) {
			//button
			if (pnt_in_rect(x, y, 150, 480, 200, 80)) {
                game_change_scene(SCENE_MENU);
                al_play_sample(button_sound, 1.2, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &button_sound_id);
            }
            else if (pnt_in_rect(x, y, 450, 480, 200, 80)) {
                game_change_scene(SCENE_START);
                al_play_sample(button_sound, 1.2, 0.0, 1.0, ALLEGRO_PLAYMODE_ONCE, &button_sound_id);
            }
		}
	}
}

void draw_movable_object(MovableObject obj) {
	if (obj.hidden)
		return;
	al_draw_bitmap(obj.img, round(obj.x - obj.w / 2), round(obj.y - obj.h / 2), 0);
}

ALLEGRO_BITMAP *load_bitmap_resized(const char *filename, int w, int h) {
	ALLEGRO_BITMAP* loaded_bmp = al_load_bitmap(filename);
	if (!loaded_bmp)
		game_abort("failed to load image: %s", filename);
	ALLEGRO_BITMAP *resized_bmp = al_create_bitmap(w, h);
	ALLEGRO_BITMAP *prev_target = al_get_target_bitmap();

	if (!resized_bmp)
		game_abort("failed to create bitmap when creating resized image: %s", filename);
	al_set_target_bitmap(resized_bmp);
	al_draw_scaled_bitmap(loaded_bmp, 0, 0,
		al_get_bitmap_width(loaded_bmp),
		al_get_bitmap_height(loaded_bmp),
		0, 0, w, h, 0);
	al_set_target_bitmap(prev_target);
	al_destroy_bitmap(loaded_bmp);

	//game_log("resized image: %s", filename);

	return resized_bmp;
}

// TODO: Define bool pnt_in_rect(int px, int py, int x, int y, int w, int h)
bool pnt_in_rect(int px, int py, int x, int y, int w, int h) {
	return px>=x && px<=x+w && py>=y && py<=y+h;
}

// +=================================================================+
// | Code below is for debugging purpose, it's fine to remove it.    |
// | Deleting the code below and removing all calls to the functions |
// | doesn't affect the game.                                        |
// +=================================================================+

void game_abort(const char* format, ...) {
	va_list arg;
	va_start(arg, format);
	game_vlog(format, arg);
	va_end(arg);
	fprintf(stderr, "error occured, exiting after 2 secs");
	// Wait 2 secs before exiting.
	al_rest(2);
	// Force exit program.
	exit(1);
}

void game_log(const char* format, ...) {
#ifdef LOG_ENABLED
	va_list arg;
	va_start(arg, format);
	game_vlog(format, arg);
	va_end(arg);
#endif
}

void game_vlog(const char* format, va_list arg) {
#ifdef LOG_ENABLED
	static bool clear_file = true;
	vprintf(format, arg);
	printf("\n");
	// Write log to file for later debugging.
	FILE* pFile = fopen("log.txt", clear_file ? "w" : "a");
	if (pFile) {
		vfprintf(pFile, format, arg);
		fprintf(pFile, "\n");
		fclose(pFile);
	}
	clear_file = false;
#endif
}
