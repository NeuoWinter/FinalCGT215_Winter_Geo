// FinalCGT215_Winter_Geo.cpp 
// Geo Winter

#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>
#include <sstream>

using namespace std;
using namespace sf;
using namespace sfp;

const float KB_SPEED = 0.2;

void LoadTex(Texture& tex, string filename) {
	if (!tex.loadFromFile(filename)) {
		cout << "Could not load " << filename << endl;
	}
}

void MoveCrossbow(PhysicsSprite& crossbow, int elapsedMS) {
	if (Keyboard::isKeyPressed(Keyboard::Right)) {
		Vector2f newPos(crossbow.getCenter());
		newPos.x = newPos.x + (KB_SPEED * elapsedMS);
		crossbow.setCenter(newPos);
	}
	if (Keyboard::isKeyPressed(Keyboard::Left)) {
		Vector2f newPos(crossbow.getCenter());
		newPos.x = newPos.x - (KB_SPEED * elapsedMS);
		crossbow.setCenter(newPos);
	}
}

Vector2f GetTextSize(Text text) {
	FloatRect r = text.getGlobalBounds();
	return Vector2f(r.width, r.height);
}

int main()
{
	// Create our window and world with gravity 0,1
	RenderWindow window(VideoMode(800, 1000), "Bucket Bouncer");
	World world(Vector2f(0, 2));

	int balls = 10;
	int score = 0;

	// Create gunner
	PhysicsSprite& crossBow = *new PhysicsSprite();
	Texture cbowTex;
	LoadTex(cbowTex, "gunner.png");
	crossBow.setTexture(cbowTex);
	Vector2f sz = crossBow.getSize();
	crossBow.setCenter(Vector2f(400, 100 - (sz.y / 2)));

	PhysicsSprite ball;
	Texture ballTex;
	LoadTex(ballTex, "ball.png");
	ball.setTexture(ballTex);
	bool drawing_ball(false);

	// ============== ARENA ===================
	// Create the left wall
	PhysicsRectangle l_wall;
	l_wall.setSize(Vector2f(500, 1000));
	l_wall.setCenter(Vector2f(-215, 500));
	l_wall.setStatic(true);
	world.AddPhysicsBody(l_wall);

	// Create the right wall
	PhysicsRectangle r_wall;
	r_wall.setSize(Vector2f(500, 1000));
	r_wall.setCenter(Vector2f(1025, 500));
	r_wall.setStatic(true);
	world.AddPhysicsBody(r_wall);

	//create floor
	PhysicsRectangle floor;
	floor.setSize(Vector2f(800, 500));
	floor.setCenter(Vector2f(400, 1300));
	floor.setStatic(true);
	world.AddPhysicsBody(floor);

	// Create the ceiling
	PhysicsRectangle ceiling;
	ceiling.setSize(Vector2f(800, 100));
	ceiling.setCenter(Vector2f(400, 10));
	ceiling.setStatic(true);
	world.AddPhysicsBody(ceiling);

	// ================ BLOCK SQUARES ===============
	// Create the center square
	PhysicsRectangle c_square;
	c_square.setSize(Vector2f(20, 20));
	c_square.setCenter(Vector2f(400, 500));
	c_square.setStatic(true);
	world.AddPhysicsBody(c_square);
	
	// Create the right square
	PhysicsRectangle r_square;
	r_square.setSize(Vector2f(20, 20));
	r_square.setCenter(Vector2f(200, 600));
	r_square.setStatic(true);
	world.AddPhysicsBody(r_square);

	// Create the left square
	PhysicsRectangle l_square;
	l_square.setSize(Vector2f(20, 20));
	l_square.setCenter(Vector2f(600, 600));
	l_square.setStatic(true);
	world.AddPhysicsBody(l_square);

	// Create the bottom_left square
	PhysicsRectangle bl_square;
	bl_square.setSize(Vector2f(20, 20));
	bl_square.setCenter(Vector2f(700, 800));
	bl_square.setStatic(true);
	world.AddPhysicsBody(bl_square);

	// Create the bottom_right square
	PhysicsRectangle br_square;
	br_square.setSize(Vector2f(20, 20));
	br_square.setCenter(Vector2f(100, 800));
	br_square.setStatic(true);
	world.AddPhysicsBody(br_square);

	Texture bucketTex;
	LoadTex(bucketTex, "bucket.png");
	PhysicsShapeList<PhysicsSprite> buckets;
	for (int i = 0; i < 1; i++) {
		PhysicsSprite& bucket = buckets.Create();
		bucket.setTexture(bucketTex);
		bucket.setSize(Vector2f(100, 100));
		Vector2f sz = bucket.getSize();
		bucket.setCenter(Vector2f(400, 900));
		bucket.setVelocity(Vector2f(0.25, -0.10));
		world.AddPhysicsBody(bucket);
		bucket.onCollision = [&drawing_ball, &world, &ball, &bucket, &buckets, &score]
		(PhysicsBodyCollisionResult result) {
			if (result.object2 == ball) {
				drawing_ball = false;
				world.RemovePhysicsBody(ball);
				score += 10;
			}
		};
	}


	// ======================== BALL COLLISIONS ===================
	// floor collision
	floor.onCollision = [&ball, &world, &balls, &drawing_ball]
	(PhysicsBodyCollisionResult result) {
		if (result.object2 == ball) {
			drawing_ball = false;
			world.RemovePhysicsBody(ball);
		}
	};


	// ============== LOAD FONT ===============
	Font fnt;
	fnt.loadFromFile("Freshman.ttf");

	Clock clock;
	Time lastTime(clock.getElapsedTime());

	while (balls > 0 || drawing_ball) {
		// calculate MS since last frame
		Time currentTime = clock.getElapsedTime();
		Time deltaTime = currentTime - lastTime;
		long deltaTimeMS = deltaTime.asMilliseconds();
		if (deltaTimeMS > 9) {
			lastTime = currentTime;
			world.UpdatePhysics(deltaTimeMS);
			MoveCrossbow(crossBow, deltaTimeMS);

			if (Keyboard::isKeyPressed(Keyboard::Space) && !drawing_ball) {
				drawing_ball = true;
				balls = balls - 1;
				int x = crossBow.getCenter().x;
				int y = crossBow.getCenter().y + 50;
				ball.setCenter(Vector2f(x,y));
				ball.setVelocity(Vector2f(0, 1));
				world.AddPhysicsBody(ball);

			}
			
			window.clear();
			if (drawing_ball) {
				window.draw(ball);
			}

			for (PhysicsShape& bucket : buckets) {
				window.draw((PhysicsSprite&)bucket);
			}
			
			ball.onCollision = [&world, &ball, &crossBow]
			(PhysicsBodyCollisionResult result) {
				if (result.object2 == crossBow) {
					ball.applyImpulse(Vector2f(0, 0));
				}
				else {
					ball.applyImpulse(Vector2f(0.5, 0.5));
				}
			};

			window.draw(crossBow);
			window.draw(floor);
			window.draw(l_wall);
			window.draw(r_wall);
			window.draw(c_square);
			window.draw(r_square);
			window.draw(l_square);
			window.draw(bl_square);
			window.draw(br_square);

			Text ball_num;
			ball_num.setFont(fnt);
			ball_num.setCharacterSize(24);
			ball_num.setString("BALLS: " + to_string(balls));
			ball_num.setPosition(80, 20);
			window.draw(ball_num);

			Text score_num;
			score_num.setFont(fnt);
			score_num.setCharacterSize(24);
			score_num.setString("SCORE: " + to_string(score));
			score_num.setPosition(610, 20);
			window.draw(score_num);

			window.display();
		}

	}
	window.display();
	
	if (balls == 0) {
		Text END;
		END.setFont(fnt);
		END.setCharacterSize(24);
		END.setString("GAME OVER");
		END.setPosition(320, 400);

		window.draw(END);
		window.display();
	}
	while (true);
}