#pragma once
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <vector>
#include <math.h>

class Ball
{
private:
	float size = 14.f, abs_velocity = 0, f = 0.996;
	sf::Vector2f velocity = sf::Vector2f(0.f, 0.f), initialMousePos;
	sf::Vector2f launchVelocity = sf::Vector2f(0.f, 0.f), copy_velocity;
	bool launched = false, stroked = true;
	int dirX = 1, dirY = 1;
public:
	sf::CircleShape ball;
	sf::Sprite ball_sprite, meter_bg, meter_fg, meter_ov, pointer;
	sf::Texture ball_tex, meter_bg_tex, meter_fg_tex, meter_ov_tex, pointer_tex;
	sf::Sound charge, holeS, swing;
	sf::SoundBuffer chargebuff, holebuff, swingbuff;
	bool trigger = false;

	void init(sf::Vector2u app_size);
	void setInitialPos(sf::Vector2i pos);
	void setVelocity(float vx, float vy);
	void setLaunchVelocity(sf::Vector2i mouse);
	bool mouseOnBall(sf::Vector2i mouse);
	bool ballNotMoving(void);
	void update(float dt, sf::Vector2u app_size, bool *init_set, sf::Sprite &hole, bool *level_complete, std::vector<sf::Sprite> blocks,
		int &currStrokes);
	void renderBall(sf::RenderWindow &app);
};

