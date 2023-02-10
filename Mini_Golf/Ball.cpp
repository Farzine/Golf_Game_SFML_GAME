#include "Ball.hpp"

void Ball::init(sf::Vector2u app_size)
{
	meter_bg_tex.loadFromFile("Resources/sprites/powermeter_bg.png");
	meter_bg.setTexture(meter_bg_tex);
	meter_bg.setScale(2, 1.2);
	meter_bg.setPosition(180, app_size.y + 12);

	meter_fg_tex.loadFromFile("Resources/sprites/powermeter_fg.png");
	meter_fg.setTexture(meter_fg_tex);
	meter_fg.setScale(2, 1.2);
	meter_fg.setPosition(188, app_size.y + 16);

	meter_ov_tex.loadFromFile("Resources/sprites/powermeter_overlay.png");
	meter_ov.setTexture(meter_ov_tex);
	meter_ov.setScale(2, 1.2);
	meter_ov.setPosition(180, app_size.y + 12);

	ball_tex.loadFromFile("Resources/sprites/ball.png");
	ball_sprite.setTexture(ball_tex);
	ball_sprite.setScale(2, 2);
	ball.setRadius(size);
	ball.setFillColor(sf::Color::White);
	ball.setPosition(200, app_size.y / 2);

	pointer_tex.loadFromFile("Resources/sprites/point.png");
	pointer.setTexture(pointer_tex);
	pointer.setScale(2, 1.2);
	pointer.setOrigin(pointer.getGlobalBounds().left, pointer.getGlobalBounds().height / 2);

	chargebuff.loadFromFile("Resources/Sounds/charge.wav");
	charge.setBuffer(chargebuff);
	swingbuff.loadFromFile("Resources/Sounds/swing.wav");
	swing.setBuffer(swingbuff);
	holebuff.loadFromFile("Resources/Sounds/hole.wav");
	holeS.setBuffer(holebuff);
}

void Ball::setInitialPos(sf::Vector2i pos)
{
	initialMousePos.x = pos.x;
	initialMousePos.y = pos.y;
}

void Ball::setLaunchVelocity(sf::Vector2i mouse)
{
	stroked = false;
	trigger = false;
	launchVelocity.x = (initialMousePos.x - mouse.x)*2;
	launchVelocity.y = (initialMousePos.y - mouse.y)*2;
	abs_velocity = sqrt(pow(launchVelocity.x, 2) + pow(launchVelocity.y, 2));

	dirX = launchVelocity.x / abs(launchVelocity.x);
	dirY = launchVelocity.y / abs(launchVelocity.y);

	sf::Vector2f normalised = launchVelocity / abs_velocity;
	float angle = atan2(normalised.y , normalised.x);
	angle = angle * (180 / 3.1415);
	pointer.setRotation(angle);
	pointer.setPosition(ball.getPosition().x + size, ball.getPosition().y + size + 4);//+4 for adjusments

	if (abs_velocity > 420) {
		launchVelocity = copy_velocity;
		return;
	}
	copy_velocity = launchVelocity;
	meter_fg.setScale(abs_velocity/200, 1.2);
}

void Ball::setVelocity(float vx, float vy)
{
	velocity.x = vx;
	velocity.y = vy;
}

bool Ball::mouseOnBall(sf::Vector2i mouse)
{
	return (mouse.x > ball.getPosition().x && mouse.x < ball.getPosition().x + size &&
		mouse.y > ball.getPosition().y && mouse.y < ball.getPosition().y + size);
}

bool Ball::ballNotMoving(void)
{
	if (velocity.x == 0 && velocity.y == 0)
		return true;
	return false;
}

void Ball::update(float dt, sf::Vector2u app_size, bool *init_set, sf::Sprite &hole, bool *level_complete, std::vector<sf::Sprite> blocks, int &currStrokes)
{
	if (!stroked) {	//initially true
		swing.play();
		launched = false;
		currStrokes--;
		if (currStrokes == 0)
			trigger = true;
		setVelocity(launchVelocity.x, launchVelocity.y);
		launchVelocity = sf::Vector2f(0.f, 0.f);
		abs_velocity = 0;
		meter_fg.setScale(abs_velocity / 200, 1.2);
		stroked = true;
	}

	//if ball is launched the update ball velocity
	float x = ball.getPosition().x + velocity.x*dt*10;
	float y = ball.getPosition().y + velocity.y*dt*10;

	//if ball collides with hole
	if (x >= hole.getPosition().x && x <= hole.getPosition().x + hole.getGlobalBounds().width &&
		y >= hole.getPosition().y && y <= hole.getPosition().y + hole.getGlobalBounds().height) {
		holeS.play();
		if (!*level_complete) {
			velocity = sf::Vector2f(0.f, 0.f);
			ball.setPosition(100, app_size.y / 2);
			*level_complete = true;
			return;
		}
	}
		
	//collision with walls
	if (x >= app_size.x - size*2) {
		velocity.x = -velocity.x;
		x = app_size.x - size*2;
		dirX = -1;
	} else if (x <= 0) {
		velocity.x = -velocity.x;
		x = 0;
		dirX = 1;
	}
	if (y >= app_size.y - size*2) {
		velocity.y = -velocity.y;
		y = app_size.y - size*2;
		dirY = -1;
	} else if (y <= 0) {
		velocity.y = -velocity.y;
		y = 0;
		dirY = 1;
	}

	//collision with blocks or tiles
	for (int i = 0; i < blocks.size(); i++) {
		float nx = x + velocity.x * dt * 10;
		if (nx + size * 2 >= blocks[i].getPosition().x && nx <= blocks[i].getPosition().x + blocks[i].getGlobalBounds().width &
			y + size * 2 >= blocks[i].getPosition().y && y <= blocks[i].getPosition().y + blocks[i].getGlobalBounds().height) {
			setVelocity(velocity.x*-1, velocity.y);
			dirX *= -1;
		}
			
		float ny = y + velocity.y * dt * 10;
		if (x + size * 2 >= blocks[i].getPosition().x && x <= blocks[i].getPosition().x + blocks[i].getGlobalBounds().width &
			ny + size * 2 >= blocks[i].getPosition().y && ny <= blocks[i].getPosition().y + blocks[i].getGlobalBounds().height) {
			setVelocity(velocity.x, velocity.y*-1);
			dirY *= -1;
		}
			
	}
	
	velocity.x = abs(velocity.x) * dirX * dt * 58;
	velocity.y = abs(velocity.y) * dirY * dt * 58;
	
	if (abs(int(velocity.x)) == 0 && abs(int(velocity.y)) == 0) {
		velocity = sf::Vector2f(0.f, 0.f);
		*init_set = true;
	}
	ball.setPosition(x, y);

}

void Ball::renderBall(sf::RenderWindow & app)
{
	if(!stroked)
		app.draw(pointer);
	app.draw(ball_sprite);
	app.draw(meter_bg);
	app.draw(meter_fg);
	app.draw(meter_ov);
}
