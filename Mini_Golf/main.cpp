
#include <sstream>
#include "Ball.hpp"
#include "Ui.hpp"

// initializing obstacle
sf::Sprite tiles32[2], tiles64[2];
sf::Texture tiles[4];
std::vector<sf::Sprite> blocks;

void loadLevel(int &currentLevel, sf::Sprite &hole, int &currentStrokes, Ball &b, sf::Vector2u app_size, int &maxStrokes);
void loadTiles(void);
bool replayButton(sf::Vector2i mousePos, bool mousePressed);

int main()
{
	srand(time(NULL));
	sf::Vector2u app_size(1280, 700);
	sf::RenderWindow app(sf::VideoMode(app_size.x, app_size.y + 40), "Mini Golf", sf::Style::Close);
	sf::Vector2i mousepos;
	bool mousePressed = false;
	sf::Clock clk;
	sf::Time time;
	float dt = 0, levelEndTimer = 0;

	//Initialization and work of hole image
	//xyz
	sf::Sprite hole;
	sf::Texture hole_tex;
	hole_tex.loadFromFile("Resources/sprites/hole.png");
	hole.setTexture(hole_tex);
	hole.setScale(3, 3);


	bool init_set = true, levelComplete = false, infoOn = true; // change info into false to hide the info of the game
	int currentLevel = 1, maxStrokes = 1, currentStrokes = maxStrokes; 

	Ball golfBall;
	golfBall.init(app_size);

	//Level
	loadTiles();
	loadLevel(currentLevel, hole, currentStrokes, golfBall, app_size, maxStrokes);
	
	//Ui
	loadUi(app_size);

	app.setVerticalSyncEnabled(true);

	while (app.isOpen())
	{
		time = clk.restart();
		dt = time.asSeconds();

		sf::Event e;
		while (app.pollEvent(e))
		{
			switch (e.type)
			{
			case sf::Event::Closed:
				app.close();
			case sf::Event::MouseButtonPressed:
				mousePressed = true;
				if (start_state) 
				{
					start_state = false;
					currentStrokes = maxStrokes + 1;  
				}
				break;
			case sf::Event::MouseButtonReleased:
				mousePressed = false;
				break;
			case sf::Event::KeyPressed:
				if(e.key.code == sf::Keyboard::R)
					golfBall.ball.setPosition(200, app_size.y / 2);
				if (e.key.code == sf::Keyboard::Enter ) // click enter  to clear the commannd text
					if (infoOn) 
					{
						infoOn = false;
						infoSound.play();
					}
				break;
			default:
				break;
			}
		}

		//////////////////////// Level Task ///////////////////////////
		if (!start_state && !finished_state) 
		{
			mousepos = sf::Mouse::getPosition(app);
			golfBall.ball_sprite.setPosition(golfBall.ball.getPosition());

			if (replayButton(mousepos, mousePressed)) 
			{
				golfBall.setVelocity(0.f, 0.f);
				mousePressed = false;
				loadLevel(currentLevel, hole, currentStrokes, golfBall, app_size, maxStrokes);
			}

			std::stringstream strokes, level;
			strokes << currentStrokes;
			strokesLeft.setString(strokes.str());
			level << currentLevel;
			currLevel.setString(level.str());

			if (levelComplete) 
			{
				blocks.clear();	//clearing the tiles
				levelEndTimer += dt;
				if (levelEndTimer > 2)
				{
					levelComplete = false;
					levelEndTimer = 0;
					currentLevel++;
					golfBall.ball_sprite.setScale(2, 2);
					loadLevel(currentLevel, hole, currentStrokes, golfBall, app_size, maxStrokes);
				}
			}
			else if(golfBall.trigger && golfBall.ballNotMoving())
			{
				blocks.clear();	//clearing the tiles 
				levelEndTimer += dt;
				if (levelEndTimer > 2) {
					levelEndTimer = 0;
					golfBall.ball_sprite.setScale(2, 2);
					loadLevel(currentLevel, hole, currentStrokes, golfBall, app_size, maxStrokes);
				}
			}
			else if (!levelComplete) 
			{
				if (mousePressed && init_set) //setting initial mouse pos
				{	
					golfBall.charge.play();
					golfBall.setInitialPos(mousepos);
					init_set = false;
				}
				else if (mousePressed && golfBall.ballNotMoving()) 
				{
					golfBall.setLaunchVelocity(mousepos);
				}
				else	//launch the ball and collision detections
					golfBall.update(dt, app_size, &init_set, hole, &levelComplete, blocks, currentStrokes);
			}
		}

		/////////////////////////////////////////////////////////////////

		app.clear();

		if (!start_state && !finished_state) 
		{
			app.draw(bg);
			app.draw(ui_bg);
			app.draw(hole);
			for (int i = 0; i < blocks.size(); i++)
				app.draw(blocks[i]);
			if (infoOn) 
			{
				for (int i = 0; i < 5; i++)
					app.draw(info[i]);
			}
			
			golfBall.renderBall(app);
		}
		renderUi(app, levelComplete, currentStrokes, golfBall);

		app.display();
	}
}

void loadLevel(int &currentLevel, sf::Sprite &hole, int &currentStrokes, Ball &b, sf::Vector2u app_size, int &maxStrokes)
{
	int i = 0;
	b.trigger = false;
	currentStrokes = maxStrokes;
	hole.setPosition(1000, 330);
	b.ball.setPosition(200, app_size.y / 2);

	// level wise tiles position
	switch (currentLevel)
	{
	case 1:
		hole.setPosition(1000, 330);
		break;

	case 2:
		//big tiles
		i = rand() % 2;
		tiles64[i].setPosition(640 - 32, 350 - 32);
		blocks.push_back(tiles64[i]);
		break;

	case 3:
		//big tiles
		i = rand() % 2;
		tiles64[i].setPosition(app_size.x / 2 - 82, 50);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x / 2 + 18, app_size.y - 114);
		blocks.push_back(tiles64[i]);
		
		//small tiles
		i = rand() % 2;
		tiles32[i].setPosition(app_size.x / 2 - 16, app_size.y / 2 - 16);
		blocks.push_back(tiles32[i]);
		break;

	case 4:
		maxStrokes = 2;
		currentStrokes = maxStrokes;
		hole.setPosition(1000, 330);
		
		//big tiles
		i = rand() % 2;
		tiles64[i].setPosition(app_size.x - 500, 50);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x - 500, app_size.y - (80 + 40));
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x - 345, app_size.y/2 - 90);
		blocks.push_back(tiles64[i]);
		
		//small tiles
		i = rand() % 2;
		tiles32[i].setPosition(500 - 16, app_size.y / 2 - 30);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(app_size.x - 500 - 16, app_size.y / 2 + 10);
		blocks.push_back(tiles32[i]);
		break;

	case 5:
		maxStrokes = 2;
		currentStrokes = maxStrokes;
		hole.setPosition(1000,330);
		
		//big tiles
		i = rand() % 2;
		tiles64[i].setPosition(app_size.x / 2 + 100 - 32, app_size.y / 2 - 32 - 100);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x / 2 - 200 - 32, 630);
		blocks.push_back(tiles64[i]);

		//big tiles 2nd part
		i = rand() % 2;
		tiles64[i].setPosition(app_size.x / 2 - 200 - 32, 5);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x / 2 + 180 - 32, app_size.y / 2 - 32 - 500);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x / 2 + 180 - 32, app_size.y / 2 - 32);
		blocks.push_back(tiles64[i]);
		
		//small tiles
		i = rand() % 2;
		tiles32[i].setPosition(200, app_size.y / 2 + 60);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(200, app_size.y / 2 - 60);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(480, app_size.y / 2 + 0);
		blocks.push_back(tiles32[i]);
		
		//big tiles 3rd part
		i = rand() % 2;
		tiles64[i].setPosition(app_size.x - 64, 0);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x - 64, app_size.y - 64);
		blocks.push_back(tiles64[i]);
		break;

	case 6:
		maxStrokes = 3;
		currentStrokes = maxStrokes;
		hole.setPosition(1000, 330);
		
		//big tiles
		i = rand() % 2;
		tiles64[i].setPosition(app_size.x / 2 - 32, 90);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x / 2 - 32, 170);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x / 2 - 32, 250);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x / 2 - 32, 330);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x / 2 - 32, 410);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x / 2 - 32, 490);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x / 2 - 32, 570);
		blocks.push_back(tiles64[i]);

		//small tiles
		i = rand() % 2;
		tiles32[i].setPosition(160, app_size.y / 2 - 50);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(200, app_size.y / 2 - 50);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(240, app_size.y / 2 - 50);
		blocks.push_back(tiles32[i]);

		//big tiles second layer
		i = rand() % 2;
		tiles64[i].setPosition(app_size.x - 460 - 32, 10);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x - 460 - 32, 80);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x - 460 - 32, 150);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x - 460 - 32, 220);
		blocks.push_back(tiles64[i]);

		//big tiles third layer
		i = rand() % 2;
		tiles64[i].setPosition(app_size.x - 460 - 32, app_size.y - 70);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x - 460 - 32, app_size.y - 140);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x - 460 - 32, app_size.y - 210);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x - 460 - 32, app_size.y - 280);
		blocks.push_back(tiles64[i]);

		break;
	case 7:
		maxStrokes = 4;
		currentStrokes = maxStrokes;
		b.ball.setPosition(190, app_size.y / 2);
		
		//small tiles, v shape part 1
		i = rand() % 2;
		tiles32[i].setPosition(140, app_size.y / 2 - 30);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(170, app_size.y / 2 - 90);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(200, app_size.y / 2 - 150);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(230, app_size.y / 2 - 210);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(260, app_size.y / 2 - 270);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(290, app_size.y / 2 - 330);
		blocks.push_back(tiles32[i]);
		
		//small tiles , v shape part 2
		i = rand() % 2;
		tiles32[i].setPosition(250, app_size.y / 2 + 10);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(280, app_size.y / 2 - 50);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(310, app_size.y / 2 - 110);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(340, app_size.y / 2 - 170);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(370, app_size.y / 2 - 230);
		blocks.push_back(tiles32[i]);

		//small tiles part 3
		i = rand() % 2;
		tiles32[i].setPosition(450, 0);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(480, 60);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(510, 120);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(540, 180);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(570, 240);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(600, 300);
		blocks.push_back(tiles32[i]);
		
		//small tiles part 4
		i = rand() % 2;
		tiles32[i].setPosition(410, 160);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(440, 220);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(470, 280);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(500, 340);
		blocks.push_back(tiles32[i]);

		//big tiles
		i = rand() % 2;
		tiles64[i].setPosition(app_size.x / 2 - 270, app_size.y / 2 + 80);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x / 2 - 270, app_size.y / 2 + 170);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x / 2 - 270, app_size.y / 2 + 260);
		blocks.push_back(tiles64[i]);

		//big tiles 
		i = rand() % 2;
		tiles64[i].setPosition(app_size.x / 2 + 150, app_size.y / 2 + 90);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x / 2 + 240, app_size.y / 2 + 90);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x / 2 + 330, app_size.y / 2 + 90);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x / 2 + 410, app_size.y / 2 + 90);
		blocks.push_back(tiles64[i]);
		tiles64[i].setPosition(app_size.x / 2 + 500, app_size.y / 2 + 90);
		blocks.push_back(tiles64[i]);
		break;
		
	case 8:
		maxStrokes = 4;
		currentStrokes = maxStrokes;
		b.ball.setPosition(190, app_size.y / 2);

		//small tiles, v shape part 1
		i = rand() % 2;
		tiles32[i].setPosition(700, app_size.y / 2 - 100);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(800, app_size.y / 2 - 150);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(900, app_size.y / 2 - 200);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(1000, app_size.y / 2 - 250);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(1100, app_size.y / 2 - 300);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(1200, app_size.y / 2 - 350);
		blocks.push_back(tiles32[i]);

		tiles32[i].setPosition(700, app_size.y / 2 + 70);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(800, app_size.y / 2 + 120);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(900, app_size.y / 2 + 170);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(1000, app_size.y / 2 + 220);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(1100, app_size.y / 2 + 270);
		blocks.push_back(tiles32[i]);
		tiles32[i].setPosition(1200, app_size.y / 2 + 320);
		blocks.push_back(tiles32[i]);

		tiles64[i].setPosition(900, 330);
		blocks.push_back(tiles64[i]);

		break;

	default:
		finished_state = true;
		break;
	}
}

void loadTiles(void)
{
	tiles[0].loadFromFile("Resources/sprites/tile32_dark.png");
	tiles[1].loadFromFile("Resources/sprites/tile32_light.png");
	tiles[2].loadFromFile("Resources/sprites/tile64_dark.png");
	tiles[3].loadFromFile("Resources/sprites/tile64_light.png");

	for (int i = 0; i < 2; i++)
		tiles32[i].setTexture(tiles[i]);
	for (int i = 2, j = 0; i < 4; i++, j++)
		tiles64[j].setTexture(tiles[i]);
}

bool replayButton(sf::Vector2i mousePos, bool mousePressed)
{
	replay.setColor(sf::Color(255,255,255,255));
	int hot = mousePos.x > replay.getPosition().x && mousePos.x < replay.getPosition().x + replay.getGlobalBounds().width &&
		mousePos.y > replay.getPosition().y && mousePos.y < replay.getPosition().y + replay.getGlobalBounds().height;
	if (hot)
		replay.setColor(sf::Color(150, 0, 0, 255));
	if (hot && mousePressed)
		return true;
	else
		return false;
}

