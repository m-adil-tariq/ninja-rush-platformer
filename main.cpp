#include <SFML/Graphics.hpp>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

using namespace std;
using namespace sf;

float randF(float min = 60.0f, float max = 100.0f)
{
    return min + static_cast<float>(rand()) / static_cast<float>(RAND_MAX / (max - min));
}

int strToInt(const string& str)
{
    int num = 0;
    stringstream ss(str);
    ss >> num;
    return num;
}

string intToStr(int num) 
{
    stringstream ss;
    ss << num;
    return ss.str();
}

int main()
{
    RenderWindow window(VideoMode::getDesktopMode(), "PROJECT NINJA RUSH", Style::Fullscreen);
    window.setFramerateLimit(60);

    enum screen { intro, playing, end };
    screen currentScreen = intro;

    fstream file;
    Font font;
    font.loadFromFile("bold.ttf");
    
    file.open("score.txt", ios::in);
    string lastHighScore = "0";
    string currentScoreStr = "0";
    string currentUser;
    int highScore = INT_MIN;

    while (file >> currentUser >> currentScoreStr)
    {
        if (strToInt(currentScoreStr) > highScore)
            highScore = strToInt(currentScoreStr);
    }
    file.close();

    Text logo("NINJA RUSH", font, window.getSize().y * 0.15f);
    logo.setPosition((window.getSize().x - logo.getGlobalBounds().width) / 2, window.getSize().y * 0.1f);

    Text scoreText("", font, window.getSize().y * 0.03f);
    scoreText.setPosition(
    (window.getSize().x - scoreText.getGlobalBounds().width) / 2,
    logo.getPosition().y + logo.getGlobalBounds().height + window.getSize().y * 0.05f
);

    Text introText("Please Enter Your Name:", font, window.getSize().y * 0.04f);
    introText.setPosition((window.getSize().x - introText.getGlobalBounds().width) / 2, window.getSize().y * 0.45f);

    Text inputText("", font, window.getSize().y * 0.05f);
    inputText.setPosition((window.getSize().x - inputText.getGlobalBounds().width) / 2, window.getSize().y * 0.55f);

    string userName = "";
    bool isKeyPressed = false;

    Text endText("", font, window.getSize().y * 0.1f);
    endText.setPosition((window.getSize().x - endText.getGlobalBounds().width) / 2, window.getSize().y * 0.3f);

    float groundHeight = window.getSize().y * 0.05f;
    RectangleShape surface(Vector2f(window.getSize().x, groundHeight));
    surface.setFillColor(Color::Green);
    surface.setPosition(0, window.getSize().y - surface.getSize().y);

    Texture ninjaTexture;
    if (!ninjaTexture.loadFromFile("guy.png"))
    {
        cerr << "Error loading ninja texture!" << endl;
        return -1;
    }

    Sprite ninja(ninjaTexture);
	float ninjaScaleFactor = (window.getSize().y * 0.18f) / ninjaTexture.getSize().y;
    ninja.setScale(ninjaScaleFactor, ninjaScaleFactor);

    ninja.setPosition(
        window.getSize().x * 0.1f,
        (window.getSize().y - surface.getSize().y) - ninja.getGlobalBounds().height + 20.0f
    );

    float obstacleWidth = window.getSize().x * 0.03f;
    float obstacleHeight = window.getSize().y * 0.06f;
    RectangleShape obstacle(Vector2f(obstacleWidth, obstacleHeight));
    obstacle.setFillColor(Color::Blue);
    obstacle.setPosition(window.getSize().x - obstacle.getSize().x, window.getSize().y - surface.getSize().y - obstacle.getSize().y);

    bool isJumping = false, isFalling = false;
    float jumpY = window.getSize().y * 0.5f, currentY;
    float jumpSpeed = window.getSize().y * 0.015f, obstacleSpeed = window.getSize().x * 0.008f;
    float groundY = (window.getSize().y - surface.getSize().y) - ninja.getGlobalBounds().height + 10.0f;
    int currentScore = 0;
    string scoreOutput;

    while (window.isOpen())
    {
        Event event;
        while (window.pollEvent(event))
        {
            if (event.type == Event::Closed)
            {
                window.close();
            }
            if (event.type == Event::KeyPressed && event.key.code == Keyboard::Escape)
            {
                if (currentScreen == end)
                {
                    file.open("score.txt", ios::out | ios::app);
                    file << "\t" << currentScore << endl;
                    file.close();
                    window.close();
                }
                else if (currentScreen == intro)
                {
                    window.close();
                }
                else
                    currentScreen = end;
            }
        }

        if (currentScreen == intro)
        {
            if (event.type == sf::Event::TextEntered) 
            {
                if (event.text.unicode < 128) 
                {
                    if (event.text.unicode == 8 && userName.length() > 0 && !isKeyPressed) 
                    {
                        userName.erase(userName.length() - 1, 1);
                        isKeyPressed = true;
                    } 
                    else if (!isKeyPressed && event.text.unicode != 13 && event.text.unicode != 32) 
                    {
                        userName += static_cast<char>(event.text.unicode);
                        isKeyPressed = true;
                    } 
                    else if (!isKeyPressed && event.text.unicode == 32) 
                    {
                        userName += '_';
                        isKeyPressed = true;
                    }
                    inputText.setString(userName);
                    inputText.setPosition((window.getSize().x - inputText.getGlobalBounds().width) / 2, window.getSize().y * 0.55f);
                }
            }
            if (event.type == sf::Event::KeyReleased) 
            {
                isKeyPressed = false;
            }
            if (event.text.unicode == 13)
            {
                file.open("score.txt", ios::in);
                while (file >> currentUser >> currentScoreStr)
                {
                    if (userName == currentUser && strToInt(currentScoreStr) > strToInt(lastHighScore))
                    {
                        lastHighScore = currentScoreStr;
                    }
                }
                file.close();

                file.open("score.txt", ios::out | ios::app);
                file << userName;
                file.close();
                currentScreen = playing;
            }

            window.clear(Color::Black);
            window.draw(logo);
            window.draw(inputText);
            window.draw(introText);
        }

        if (currentScreen == playing)
        {
            if (Keyboard::isKeyPressed(Keyboard::Up) && !isJumping && !isFalling)
            {
                isJumping = true;
            }
            if (isJumping)
            {
                if (ninja.getPosition().y > jumpY)
                {
                    ninja.move(0.0f, -jumpSpeed);
                }
                else
                {
                    isJumping = false;
                    isFalling = true;
                }
            }
            else if (isFalling)
            {
                if (ninja.getPosition().y < groundY)
                {
                    ninja.move(0.0f, jumpSpeed);
                }
                else
                {
                    isFalling = false;
                }
            }

            if (obstacle.getPosition().x + obstacle.getSize().x > 0)
            {
                obstacle.move(-obstacleSpeed, 0.0f);
            }
            else
            {
                float obstacleWidth = window.getSize().x * randF(0.02f, 0.05f);
				float obstacleHeight = window.getSize().y * randF(0.05f, 0.12f);
				obstacle.setSize(Vector2f(obstacleWidth, obstacleHeight));
				obstacle.setPosition(
				    window.getSize().x - obstacle.getSize().x,
				    window.getSize().y - surface.getSize().y - obstacle.getSize().y
				);
				obstacle.setFillColor(Color(rand() % 256, rand() % 256, rand() % 256));
                currentScore++;
                if (currentScore % 7 == 0)
                {
                    obstacleSpeed += window.getSize().x * 0.002f;
                    jumpSpeed += window.getSize().y * 0.002f;
                }
            }

            FloatRect ninjaBounds
            (
                ninja.getGlobalBounds().left + 20.0f,
                ninja.getGlobalBounds().top + 20.0f,
                ninja.getGlobalBounds().width - 50.0f,
                ninja.getGlobalBounds().height - 30.0f
            );

            if (ninjaBounds.intersects(obstacle.getGlobalBounds()))
            {
                currentScreen = end;
            }

            scoreOutput = "Score: " + intToStr(currentScore) + "\t Your Last High Score: " + lastHighScore + "\t Overall High Score: " + intToStr(highScore);
            scoreText.setString(scoreOutput);
            scoreText.setPosition(
			    (window.getSize().x - scoreText.getGlobalBounds().width) / 2,
			    logo.getPosition().y + logo.getGlobalBounds().height + window.getSize().y * 0.05f
			);

            window.clear(Color::Black);
            window.draw(logo);
            window.draw(obstacle);
            window.draw(scoreText);
            window.draw(ninja);
        }
        else if (currentScreen == end)
        {
            endText.setString("Game  End!!!\nYour Score: " + intToStr(currentScore));
            endText.setPosition((window.getSize().x - endText.getGlobalBounds().width) / 2, window.getSize().y * 0.3f);
            window.clear(Color::Red);
            window.draw(endText);
        }

        window.draw(surface);
        window.display();
    }

    return 0;
}

