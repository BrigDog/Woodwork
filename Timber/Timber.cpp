//Include libaries here.
#include <fstream>
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <list>
#include <string>  
#include <cmath>
//Make code easier to write with "using namespace"
using namespace sf;
//Time variable
Time dt;
bool treeMove = false;
int treeMoveOnce = 0;
bool playerPos = false;
int branchSpawnPos = 0;
bool spawnAnotherBranch = true;
bool reset = true;
std::string line;
int globalHighScore;

//All my data on logs applied here and called as an instance.
struct logData
{
    Texture logTexture;
    Sprite logSprites[9];
    int logGoals[9];
};
//All my data on branches applied here and called as an instance.
struct branchData
{
    Texture branchTexture;
    Sprite branchSprites[4];
    int branchGoals[4];
    bool branchFalls[4];
};
//All my data on clouds applied here and called as an instance.
struct cloudData
{
    Texture cloudTextures[3];
    Sprite cloudSprites[3];
    bool cloudActive[3];
    float cloudSpeed[3];
};

struct playerData
{
    Texture playerTextures[3];
    Sprite playerSprite;
    int whichSkin;
};

struct numberData
{
    int score;
    Texture numberTextures[10];
    std::list<Sprite> digitSprites;
};

struct timeBarData
{
    Texture backBarTextures[5];
    Texture frontBarTexture;
    Sprite backBarSprite;
    float timeLeft;
};

sf::Vector2f round(const sf::Vector2f vector)
{
    return sf::Vector2f{ std::round(vector.x), std::round(vector.y) };
}

//This is where the game starts from.
int main()
{
    //Create a video mode object.
    VideoMode vm(240, 160);

    //Create and open a window for the game.
    RenderWindow window(vm, "Woodwork", Style::Default);

    struct logData logD;
    struct branchData branchD;
    struct cloudData cloudD;
    struct cloudData menuCloudD;
    struct playerData playerD;
    struct playerData menuPlayerD;
    struct timeBarData timeBarD;

    void cloudMovement(cloudData&);
    void birdMovement(bool&, int&, Sprite&, IntRect&, float&);

    Clock clock;

    int characterPicker = 0;
    int mapPicker = 0;

    String gameState = "Menu";
    while (window.isOpen())
    {
        if (gameState == "Menu")
        {
            int selectPosition = 50;
            int counter = 0;
            float resetTime = 0;

            //Make a background.
                //Create a texture to hold a graphic on the GPU.
            Texture textureBackground;
            //Load a graphic into the texture.
            if (mapPicker == 0)
            {
                textureBackground.loadFromFile("graphics/background.png");
            }
            else
            {
                textureBackground.loadFromFile("graphics/background2.png");
            }
            //Create a sprite
            Sprite spriteBackground;
            //Attach the texture to the sprite.
            spriteBackground.setTexture(textureBackground);
            //Set the position of the sprite.
            spriteBackground.setPosition(0, 0);

            //Make a midground.
            //Create a texture to hold a graphic on the GPU.
            Texture textureMidground;
            //Load a graphic into the texture.
            if (mapPicker == 0)
            {
                textureMidground.loadFromFile("graphics/midground.png");
            }
            else
            {
                textureMidground.loadFromFile("graphics/midground2.png");
            }
            //Create a sprite
            Sprite spriteMidground;
            //Attach the texture to the sprite.
            spriteMidground.setTexture(textureMidground);
            //Set the position of the sprite.
            spriteMidground.setPosition(0, 0);

            //Make a menu selector.
                //Create a texture to hold a graphic on the GPU.
            Texture textureMenuSelect;
            //Load a graphic into the texture.
            textureMenuSelect.loadFromFile("graphics/menuSelecter.png");
            //Create a sprite
            Sprite spriteMenuSelect;
            //Attach the texture to the sprite.
            spriteMenuSelect.setTexture(textureMenuSelect);
            //Set the position of the sprite.
            spriteMenuSelect.setPosition(0, 70);

            //Create a bird sprite.
            Texture menuBirdTexture;
            menuBirdTexture.loadFromFile("graphics/birdSpritesheet.png");
            sf::IntRect menuBirdSpriteRect(0, 0, 10, 14);
            sf::Sprite menuBirdSprite(menuBirdTexture, menuBirdSpriteRect);
            menuBirdSprite.setPosition(240, 0);
            bool menuBirdActive = false;
            int menuBirdSpeed = 5;
            float menuBirdTimer = 0;
            //Create a cloud sprite.
            for (counter = 0; counter < 3; counter++)
            {
                switch (counter) {
                case 0:
                    menuCloudD.cloudTextures[counter].loadFromFile("graphics/randomCloud1.png");
                    break;
                case 1:
                    menuCloudD.cloudTextures[counter].loadFromFile("graphics/randomCloud2.png");
                    break;
                case 2:
                    menuCloudD.cloudTextures[counter].loadFromFile("graphics/randomCloud3.png");
                    break;
                }
                menuCloudD.cloudSprites[counter].setTexture(menuCloudD.cloudTextures[counter]);
                branchD.branchFalls[counter] = false;
                bool cloud1Active = true;
                menuCloudD.cloudActive[counter] = false;
                menuCloudD.cloudSprites[counter].setPosition(-74, 0);
            }
            counter = 0;

            sf::Font font;
            font.loadFromFile("fonts/space-harrier-extended.ttf");
            sf::Text title;
            title.setFont(font);
            title.setPosition(88, 5);
            title.setLetterSpacing(0);
            title.setLineSpacing(0);
            title.setCharacterSize(80);
            title.setScale(0.1f, 0.1f);
            title.setFillColor(sf::Color::Black);
            title.setString("WOODWORK");

            sf::Text play;
            play.setFont(font);
            play.setPosition(88, 50);
            play.setLetterSpacing(0);
            play.setLineSpacing(0);
            play.setCharacterSize(80);
            play.setScale(0.1f, 0.1f);
            play.setFillColor(sf::Color::Black);
            play.setString("PLAY");

            sf::Text customize;
            customize.setFont(font);
            customize.setPosition(88, 70);
            customize.setLetterSpacing(0);
            customize.setLineSpacing(0);
            customize.setCharacterSize(80);
            customize.setScale(0.1f, 0.1f);
            customize.setFillColor(sf::Color::Black);
            customize.setString("CUSTOMIZE");

            sf::Text quit;
            quit.setFont(font);
            quit.setPosition(88, 90);
            quit.setLetterSpacing(0);
            quit.setLineSpacing(0);
            quit.setCharacterSize(80);
            quit.setScale(0.1f, 0.1f);
            quit.setFillColor(sf::Color::Black);
            quit.setString("QUIT");

            if (characterPicker == 0) 
            {
                menuPlayerD.playerTextures[0].loadFromFile("graphics/defaultPlayer1.png");
            } else
            {
                menuPlayerD.playerTextures[0].loadFromFile("graphics/winterPlayer1.png");
            }
            menuPlayerD.playerSprite.setTexture(menuPlayerD.playerTextures[0]);
            menuPlayerD.playerSprite.setPosition(30, 100);

            while (window.isOpen() & gameState == "Menu")
            {
                dt = clock.restart();
                sf::Event _event;
                while (window.pollEvent(_event))
                {
                    if (_event.type == sf::Event::Closed) {
                        window.close();
                    }
                }
                if (Keyboard::isKeyPressed(Keyboard::Enter))
                {
                    if (reset == true)
                    {
                        reset = false;
                        if (selectPosition == 50)
                        {
                            gameState = "Game";
                            window.clear();
                        }
                        if (selectPosition == 70)
                        {
                            gameState = "Custom";
                            window.clear();
                        }
                        if (selectPosition == 90)
                        {
                            window.close();
                        }
                    }
                }
                if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S))
                {
                    if (reset == true)
                    {
                        reset = false;
                        selectPosition += 20;
                    }
                }
                if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W))
                {
                    if (reset == true)
                    {
                        reset = false;
                        selectPosition -= 20;
                    }
                }
                if (reset == false)
                {
                    resetTime += dt.asSeconds();
                    if (resetTime >= 0.15f)
                    {
                        reset = true;
                        resetTime = 0.0f;
                    }
                }
                if (selectPosition > 90)
                {
                    selectPosition = 50;
                }
                else if (selectPosition < 50)
                {
                    selectPosition = 90;
                }
                //Manage the clouds / Snow
                if (mapPicker == 0)
                {
                    menuBirdTimer += dt.asSeconds();
                    birdMovement(menuBirdActive, menuBirdSpeed, menuBirdSprite, menuBirdSpriteRect, menuBirdTimer);
                }
                cloudMovement(menuCloudD);
                spriteMenuSelect.setPosition(0, selectPosition);
                //Clear everything from the last frame.
                window.clear();

                window.draw(spriteBackground);
                for (counter = 0; counter < 3; counter++)
                {
                    window.draw(menuCloudD.cloudSprites[counter]);
                }
                if(mapPicker == 0)
                {
                    menuBirdSprite.setTextureRect(menuBirdSpriteRect);
                    window.draw(menuBirdSprite);
                }
                window.draw(spriteMidground);
                window.draw(menuPlayerD.playerSprite);
                window.draw(title);
                window.draw(spriteMenuSelect);
                window.draw(play);
                window.draw(customize);
                window.draw(quit);

                //Show everything that has been drawn.
                window.display();
            }
        }
        if (gameState == "Game")
        {
            //All local variables stored here in sections of their data type.
            void moveTreeTrunk(Sprite&, int);
            void spawnBranch(Sprite&, bool, bool&);
            void moveTreeBranch(Sprite&, int, bool&);
            void collisionCheck(Sprite&, bool&);
            void treeMovement(struct logData&, struct branchData, bool&);
            void highScore(int, std::fstream&);

            std::fstream myfile;


            bool playerIsDead = false;
            bool playerHit = false;
            bool paused = true;
            bool scoreAnimation = false;

            int counter = 0;
            int score = 0;
            int lastScore = 0;

            float maxTime = 1;
            float timeLeft = maxTime;
            float playerTime = 0;
            float scoreAnimationTime = 0;
            float waitTime = 0;

            highScore(score, myfile);

            //Making text and loading font.
            sf::Font font;
            font.loadFromFile("fonts/space-harrier-extended.ttf");
            sf::Text scoreText;
            scoreText.setFont(font);
            scoreText.setPosition(116, 5);
            scoreText.setLetterSpacing(0);
            scoreText.setLineSpacing(0);
            scoreText.setCharacterSize(80);
            scoreText.setScale(0.1f, 0.1f);
            scoreText.setFillColor(sf::Color::Black);
            scoreText.setString(std::to_string(score));

            sf::Text highScoreText;
            highScoreText.setFont(font);
            highScoreText.setPosition(5, 5);
            highScoreText.setLetterSpacing(0);
            highScoreText.setLineSpacing(0);
            highScoreText.setCharacterSize(80);
            highScoreText.setScale(0.1f, 0.1f);
            highScoreText.setFillColor(sf::Color::Black);
            highScoreText.setString(std::to_string(globalHighScore));

            //Create a log sprite.
            logD.logTexture.loadFromFile("graphics/treePart.png");
            counter = 0;
            while (counter < 9)
            {
                logD.logSprites[counter].setTexture(logD.logTexture);
                logD.logSprites[counter].setPosition(103, 111 - (18 * counter));
                counter++;
            }

            //Create a branch sprite.
            if (mapPicker == 0)
            {
                branchD.branchTexture.loadFromFile("graphics/branch.png");
            }
            else
            {
                branchD.branchTexture.loadFromFile("graphics/branch2.png");
            }
            for (counter = 0; counter < 4; counter++)
            {
                branchD.branchSprites[counter].setTexture(branchD.branchTexture);
                branchD.branchFalls[counter] = false;
                spawnBranch(branchD.branchSprites[counter], true, branchD.branchFalls[counter]);
            }

            //Create a cloud sprite.
            for (counter = 0; counter < 3; counter++)
            {
                switch (counter) {
                case 0:
                    cloudD.cloudTextures[counter].loadFromFile("graphics/randomCloud1.png");
                    break;
                case 1:
                    cloudD.cloudTextures[counter].loadFromFile("graphics/randomCloud2.png");
                    break;
                case 2:
                    cloudD.cloudTextures[counter].loadFromFile("graphics/randomCloud3.png");
                    break;
                }
                cloudD.cloudSprites[counter].setTexture(cloudD.cloudTextures[counter]);
                branchD.branchFalls[counter] = false;
                bool cloud1Active = true;
                cloudD.cloudActive[counter] = false;
                cloudD.cloudSprites[counter].setPosition(-74, 0);
            }

            //Create a player sprite.
            if (characterPicker == 0)
            {
                playerD.playerTextures[0].loadFromFile("graphics/defaultPlayer1.png");
                playerD.playerTextures[1].loadFromFile("graphics/defaultPlayer2.png");
                playerD.playerTextures[2].loadFromFile("graphics/defaultPlayer3.png");
            }
            else if (characterPicker == 1)
            {
                playerD.playerTextures[0].loadFromFile("graphics/winterPlayer1.png");
                playerD.playerTextures[1].loadFromFile("graphics/winterPlayer2.png");
                playerD.playerTextures[2].loadFromFile("graphics/winterPlayer3.png");
            }
            playerD.playerSprite.setTexture(playerD.playerTextures[0]);
            playerD.playerSprite.setPosition(65, 100);

            //Create a bird sprite.
            Texture birdTexture;
            birdTexture.loadFromFile("graphics/birdSpritesheet.png");
            sf::IntRect birdSpriteRect(0, 0, 10, 14);
            sf::Sprite birdSprite(birdTexture, birdSpriteRect);
            birdSprite.setPosition(240, 0);
            bool birdActive = false;
            int birdSpeed = 5;
            float birdTimer = 0;

            //Create a time bar.
            timeBarD.backBarTextures[0].loadFromFile("graphics/healthOutline1.png");
            timeBarD.backBarTextures[1].loadFromFile("graphics/healthOutline2.png");
            timeBarD.backBarTextures[2].loadFromFile("graphics/healthOutline3.png");
            timeBarD.backBarTextures[3].loadFromFile("graphics/healthOutline4.png");
            timeBarD.backBarTextures[4].loadFromFile("graphics/healthOutline5.png");
            timeBarD.frontBarTexture.loadFromFile("graphics/redHealth.png");
            timeBarD.backBarSprite.setTexture(timeBarD.backBarTextures[0]);
            sf::IntRect timeBarSpriteRect(0, 0, 148, 8);
            sf::Sprite timeBarSprite(timeBarD.frontBarTexture, timeBarSpriteRect);
            timeBarD.backBarSprite.setPosition(45, 145);
            timeBarSprite.setPosition(46, 146);

            //Make a chop sound.
            sf::SoundBuffer chop;
            // load something into the sound buffer...
            chop.loadFromFile("audio/chop.wav");
            sf::Sound sound;
            sound.setBuffer(chop);

            //Make a milestone sound.
            sf::SoundBuffer milestone;
            // load something into the sound buffer...
            milestone.loadFromFile("audio/milestone.ogg");
            sf::Sound milestoneSound;
            milestoneSound.setBuffer(milestone);

            //Make a background.
            //Create a texture to hold a graphic on the GPU.
            Texture textureBackground;
            //Load a graphic into the texture.
            if (mapPicker == 0)
            {
                textureBackground.loadFromFile("graphics/background.png");
            }
            else
            {
                textureBackground.loadFromFile("graphics/background2.png");
            }
            //Create a sprite
            Sprite spriteBackground;
            //Attach the texture to the sprite.
            spriteBackground.setTexture(textureBackground);
            //Set the position of the sprite.
            spriteBackground.setPosition(0, 0);

            //Make a tree stump.
            //Create a texture to hold a graphic on the GPU.
            Texture textureStump;
            //Load a graphic into the texture.
            if(mapPicker == 0)
            {
                textureStump.loadFromFile("graphics/stump.png");
            }
            else if (mapPicker == 1)
            {
                textureStump.loadFromFile("graphics/stump2.png");
            }
            //Create a sprite
            Sprite spriteStump;
            //Attach the texture to the sprite.
            spriteStump.setTexture(textureStump);
            //Set the position of the sprite.
            spriteStump.setPosition(98, 129);

            //Make a midground.
            //Create a texture to hold a graphic on the GPU.
            Texture textureMidground;
            //Load a graphic into the texture.
            if (mapPicker == 0)
            {
                textureMidground.loadFromFile("graphics/midground.png");
            }
            else
            {
                textureMidground.loadFromFile("graphics/midground2.png");
            }
            //Create a sprite
            Sprite spriteMidground;
            //Attach the texture to the sprite.
            spriteMidground.setTexture(textureMidground);
            //Set the position of the sprite.
            spriteMidground.setPosition(0, 0);

            //Make a gravestone.
            //Create a texture to hold a graphic on the GPU.
            Texture textureGravestone;
            //Load a graphic into the texture.
            if (mapPicker == 0)
            {
                textureGravestone.loadFromFile("graphics/gravestone.png");
            }
            else if (mapPicker == 1)
            {
                textureGravestone.loadFromFile("graphics/gravestone2.png");
            }
            //Create a sprite
            Sprite spriteGravestone;
            //Attach the texture to the sprite.
            spriteGravestone.setTexture(textureGravestone);
            //Set the position of the sprite.
            spriteGravestone.setPosition(73, 121);

            for (counter = 0; counter < 4; counter++)
            {
                spawnBranch(branchD.branchSprites[counter], true, branchD.branchFalls[counter]);
            }
            counter = 0;
            while (counter < 9)
            {
                logD.logSprites[counter].setTexture(logD.logTexture);
                logD.logSprites[counter].setPosition(103, 111 - (18 * counter));
                counter++;
            }

            reset = true;
            highScore(score, myfile);
            playerIsDead = false;
            playerHit = false;
            paused = true;
            counter = 0;
            score = 0;
            timeLeft = maxTime;
            scoreText.setString(std::to_string(score));
            scoreText.setPosition(116, 5);
            playerTime = 0;
            playerPos = false;
            treeMove = false;
            treeMoveOnce = 0;
            branchSpawnPos = 0;
            spawnAnotherBranch = true;
            for (counter = 0; counter < 4; counter++)
            {
                spawnBranch(branchD.branchSprites[counter], true, branchD.branchFalls[counter]);
            }
            counter = 0;
            while (counter < 9)
            {
                logD.logSprites[counter].setTexture(logD.logTexture);
                logD.logSprites[counter].setPosition(103, 111 - (18 * counter));
                counter++;
            }

            //Begins a while loop to stop the program from ending.
            while (window.isOpen() & gameState == "Game")
            {
                //Handle the application resize and close functionality
                sf::Event _event;
                while (window.pollEvent(_event))
                {
                    if (_event.type == sf::Event::Closed) {
                        highScore(score, myfile);
                        window.close();
                    }
                }

                //Handle the player input.
                if (Keyboard::isKeyPressed(Keyboard::Escape))
                {
                    highScore(score, myfile);
                    gameState = "Menu";
                    window.clear();
                }
                if (Keyboard::isKeyPressed(Keyboard::S))
                {
                    highScore(score, myfile);
                }

                if (!paused && playerIsDead == false)
                {
                    if (Keyboard::isKeyPressed(Keyboard::Space))
                    {
                        playerHit = true;
                        playerTime = 0;
                    }

                    if (Keyboard::isKeyPressed(Keyboard::D))
                    {
                        playerPos = true;
                    }

                    if (Keyboard::isKeyPressed(Keyboard::A))
                    {
                        playerPos = false;
                    }

                    if (Keyboard::isKeyPressed(Keyboard::X))
                    {
                        std::cout << branchD.branchSprites[0].getPosition().y;
                    }
                }

                //Player position checker
                if (playerPos == false)
                {
                    playerD.playerSprite.setPosition(65, 100);
                    playerD.playerSprite.setScale(1, 1);
                    spriteGravestone.setPosition(73, 121);
                } else
                {
                    playerD.playerSprite.setPosition(174, 100);
                    playerD.playerSprite.setScale(-1, 1);
                    spriteGravestone.setPosition(143, 121);
                }

                if (Keyboard::isKeyPressed(Keyboard::R) && reset == true)
                {
                    reset = false;
                    highScore(score, myfile);
                    playerIsDead = false;
                    playerHit = false;
                    paused = true;
                    counter = 0;
                    score = 0;
                    timeLeft = maxTime;
                    scoreText.setString(std::to_string(score));
                    scoreText.setPosition(116, 5);
                    playerTime = 0;
                    playerPos = false;
                    treeMove = false;
                    treeMoveOnce = 0;
                    branchSpawnPos = 0;
                    spawnAnotherBranch = true;
                    for (counter = 0; counter < 4; counter++)
                    {
                        spawnBranch(branchD.branchSprites[counter], true, branchD.branchFalls[counter]);
                    }
                    counter = 0;
                    while (counter < 9)
                    {
                        logD.logSprites[counter].setTexture(logD.logTexture);
                        logD.logSprites[counter].setPosition(103, 111 - (18 * counter));
                        counter++;
                    }
                }

                //Start the game
                if (Keyboard::isKeyPressed(Keyboard::Q) && reset == true)
                {
                    reset = false;
                    dt = clock.restart();
                    paused = !paused;
                }

                //Update the scene.
                if (reset == false)
                {
                    waitTime += dt.asSeconds();
                    if (waitTime >= 1)
                    {
                        reset = true;
                        waitTime = 0.0f;
                    }
                }
                if (!paused)
                {
                    //Measure time from each frame.
                    dt = clock.restart();

                    //Manage the player.
                    if (playerHit == true)
                    {
                        playerTime += dt.asSeconds();
                        if (playerTime > 0.3)
                        {
                            playerD.playerSprite.setTexture(playerD.playerTextures[0]);
                            playerHit = false;
                        }
                        else if (playerTime > 0.15)
                        {
                            playerD.playerSprite.setTexture(playerD.playerTextures[1]);
                        }
                        else if (playerTime > 0.05)
                        {
                            playerD.playerSprite.setTexture(playerD.playerTextures[2]);
                            if (treeMoveOnce == 0 && treeMove == false)
                            {
                                sound.play();
                                treeMoveOnce = 1;
                                treeMove = true;
                            }
                        }
                        else
                        {
                            playerD.playerSprite.setTexture(playerD.playerTextures[1]);
                        }
                    }
                    if (score % 100 == 0 && !score == 0 && lastScore != score)
                    {
                        scoreAnimation = true;
                    }
                    if (scoreAnimation == true)
                    {
                        scoreAnimationTime += dt.asSeconds();
                        if (scoreAnimationTime > 0.7)
                        {
                            timeBarD.backBarSprite.setTexture(timeBarD.backBarTextures[0]);
                            scoreAnimationTime = 0;
                            lastScore = score;
                            scoreAnimation = false;
                        }
                        else if (scoreAnimationTime > 0.6)
                        {
                            timeBarD.backBarSprite.setTexture(timeBarD.backBarTextures[1]);
                        }
                        else if (scoreAnimationTime > 0.5)
                        {
                            timeBarD.backBarSprite.setTexture(timeBarD.backBarTextures[2]);
                        }
                        else if (scoreAnimationTime > 0.4)
                        {
                            timeBarD.backBarSprite.setTexture(timeBarD.backBarTextures[3]);
                        }
                        else if (scoreAnimationTime > 0.3)
                        {
                            timeBarD.backBarSprite.setTexture(timeBarD.backBarTextures[4]);
                        }
                        else if (scoreAnimationTime > 0.2)
                        {
                            timeBarD.backBarSprite.setTexture(timeBarD.backBarTextures[3]);
                        }
                        else if (scoreAnimationTime > 0.1)
                        {
                            timeBarD.backBarSprite.setTexture(timeBarD.backBarTextures[2]);
                        }
                        else
                        {
                            milestoneSound.play();
                            timeBarD.backBarSprite.setTexture(timeBarD.backBarTextures[1]);
                        }
                    }

                    //Manage the time left bar.
                    timeLeft -= dt.asSeconds();
                    if (timeLeft <= 0)
                    {
                        timeLeft = 0;
                        playerIsDead = true;
                    }
                    if (timeLeft >= maxTime)
                    {
                        timeLeft = maxTime;
                    }

                    //Manage the tree
                    if (treeMove == true)
                    {
                        if (treeMoveOnce == 1)
                        {
                            timeLeft += 0.2f;
                            counter = 0;
                            while (counter < 9)
                            {
                                logD.logGoals[counter] = logD.logSprites[counter].getPosition().y + 18;
                                counter++;
                            }
                            for (counter = 0; counter < 4; counter++)
                            {
                                branchD.branchGoals[counter] = branchD.branchSprites[counter].getPosition().y + 18;
                            }
                            for (counter = 0; counter < 4; counter++)
                            {
                                if (branchD.branchFalls[counter] == true)
                                {
                                    spawnBranch(branchD.branchSprites[counter], false, branchD.branchFalls[counter]);
                                    counter = 4;
                                }
                            }
                            score++;
                            scoreText.setString(std::to_string(score));
                            if (score >= 100)
                            {
                                scoreText.setPosition(108, 5);
                            }
                            else if (score >= 10)
                            {
                                scoreText.setPosition(112, 5);
                            }
                            treeMoveOnce = 2;
                        }
                        counter = 0;
                        while (counter < 9)
                        {
                            moveTreeTrunk(logD.logSprites[counter], logD.logGoals[counter]);
                            counter++;
                        }

                        for (counter = 0; counter < 4; counter++)
                        {
                            moveTreeBranch(branchD.branchSprites[counter], branchD.branchGoals[counter],
                                branchD.branchFalls[counter]);
                        }
                    }

                    for (counter = 0; counter < 4; counter++)
                    {
                        if (branchD.branchFalls[counter] == true)
                        {
                            moveTreeBranch(branchD.branchSprites[counter], branchD.branchGoals[counter],
                                branchD.branchFalls[counter]);
                        }
                        collisionCheck(branchD.branchSprites[counter], playerIsDead);
                    }
                    //treeMovement(logD, branchD, playerIsDead);
                    //Manage the clouds.
                    cloudMovement(cloudD);
                    //Manage the bird.
                    birdTimer += dt.asSeconds();
                    birdMovement(birdActive, birdSpeed, birdSprite, birdSpriteRect, birdTimer);
                }
                if (playerIsDead == true)
                {
                    timeLeft = 0;
                }
                float moveTimeBar = fabsf((timeLeft / maxTime) - 1);
                sf::IntRect updatedTimeBarSpriteRect(0 + (moveTimeBar * 148), 0, 148, 8);

                //Clear everything from the last frame.
                window.clear();

                //Draw the game scene.
                window.draw(spriteBackground);
                for (counter = 0; counter < 3; counter++)
                {
                    window.draw(cloudD.cloudSprites[counter]);
                }
                window.draw(spriteMidground);
                birdSprite.setTextureRect(birdSpriteRect);
                if(mapPicker == 0)
                {
                    window.draw(birdSprite);
                }
                window.draw(spriteStump);
                counter = 0;
                while (counter < 9)
                {
                    window.draw(logD.logSprites[counter]);
                    counter++;
                }
                for (counter = 0; counter < 4; counter++)
                {
                    window.draw(branchD.branchSprites[counter]);
                }
                if (playerIsDead == false)
                {
                    window.draw(playerD.playerSprite);
                    //window.draw(spritePlayer);
                }
                else
                {
                    window.draw(spriteGravestone);
                }
                window.draw(scoreText);
                highScoreText.setString(std::to_string(globalHighScore));
                window.draw(highScoreText);
                window.draw(timeBarD.backBarSprite);
                timeBarSprite.setTextureRect(updatedTimeBarSpriteRect);
                window.draw(timeBarSprite);

                //Show everything that has been drawn.
                window.display();
            }
        }
        if (gameState == "Custom") 
        {
            //Make a background.
                //Create a texture to hold a graphic on the GPU.
            Texture textureBackground;
            //Load a graphic into the texture.
            textureBackground.loadFromFile("graphics/customizationBackground.png");
            //Create a sprite
            Sprite spriteBackground;
            //Attach the texture to the sprite.
            spriteBackground.setTexture(textureBackground);
            //Set the position of the sprite.
            spriteBackground.setPosition(0, 0);

            //Make a midground.
            //Create a texture to hold a graphic on the GPU.
            Texture textureMidground;
            //Load a graphic into the texture.
            textureMidground.loadFromFile("graphics/customizationMidground.png");
            //Create a sprite
            Sprite spriteMidground;
            //Attach the texture to the sprite.
            spriteMidground.setTexture(textureMidground);
            //Set the position of the sprite.
            spriteMidground.setPosition(0, 0);

            //Make a select.
            //Create a texture to hold a graphic on the GPU.
            Texture textureSelect;
            //Load a graphic into the texture.
            textureSelect.loadFromFile("graphics/selectedCustom.png");
            //Create a sprite
            Sprite spriteSelect;
            //Attach the texture to the sprite.
            spriteSelect.setTexture(textureSelect);
            //Set the position of the sprite.
            spriteSelect.setPosition(52, 24);

            //Make a select.
            //Create a texture to hold a graphic on the GPU.
            Texture textureActive;
            //Load a graphic into the texture.
            textureActive.loadFromFile("graphics/activeCustom.png");
            //Create a sprite
            Sprite spriteActive1;
            //Attach the texture to the sprite.
            spriteActive1.setTexture(textureActive);
            //Set the position of the sprite.
            spriteActive1.setPosition(52, 24);
            //Create a sprite
            Sprite spriteActive2;
            //Attach the texture to the sprite.
            spriteActive2.setTexture(textureActive);
            //Set the position of the sprite.
            spriteActive2.setPosition(144, 91);

            float resetTime = 0;

            int selectPositionY = 24;
            int selectPositionX = 52;

            while (window.isOpen() & gameState == "Custom")
            {
                if (reset == false)
                {
                    resetTime += dt.asSeconds();
                    if (resetTime >= 0.2f)
                    {
                        reset = true;
                        resetTime = 0.0f;
                    }
                }

                sf::Event _event;
                while (window.pollEvent(_event))
                {
                    if (_event.type == sf::Event::Closed) {
                        window.close();
                    }
                }

                if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S))
                {
                    if (reset == true)
                    {
                        reset = false;
                        selectPositionY += 67;
                    }
                }
                if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W))
                {
                    if (reset == true)
                    {
                        reset = false;
                        selectPositionY -= 67;
                    }
                }
                if (selectPositionY > 92)
                {
                    selectPositionY = 24;
                }
                else if (selectPositionY < 23)
                {
                    selectPositionY = 91;
                }
                
                if (Keyboard::isKeyPressed(Keyboard::Right) || Keyboard::isKeyPressed(Keyboard::D))
                {
                    if (reset == true)
                    {
                        reset = false;
                        selectPositionX += 92;
                    }
                }
                if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A))
                {
                    if (reset == true)
                    {
                        reset = false;
                        selectPositionX -= 92;
                    }
                }
                if (selectPositionX > 145)
                {
                    selectPositionX = 52;
                }
                else if (selectPositionX < 51)
                {
                    selectPositionX = 144;
                }

                if (Keyboard::isKeyPressed(Keyboard::Enter))
                {
                    if (reset == true)
                    {
                        reset = false;
                        if (selectPositionY == 24 & selectPositionX == 52)
                        {
                            spriteActive1.setPosition(52, 24);
                            characterPicker = 0;
                        }
                        if (selectPositionY == 24 & selectPositionX == 144)
                        {
                            spriteActive1.setPosition(144, 24);
                            characterPicker = 1;
                        }
                        if (selectPositionY == 91 & selectPositionX == 52)
                        {
                            spriteActive2.setPosition(52, 91);
                            mapPicker = 1;
                        }
                        if (selectPositionY == 91 & selectPositionX == 144)
                        {
                            spriteActive2.setPosition(144, 91);
                            mapPicker = 0;
                        }
                    }
                    
                }
                if (Keyboard::isKeyPressed(Keyboard::Escape))
                {
                    gameState = "Menu";
                    window.clear();
                }
                if (characterPicker == 1)
                {
                    spriteActive1.setPosition(144, 24);
                }
                if (mapPicker == 1)
                {
                    spriteActive2.setPosition(52, 91);
                }
                spriteSelect.setPosition(selectPositionX, selectPositionY);
                //Clear everything from the last frame.
                window.clear();

                window.draw(spriteBackground);
                window.draw(spriteMidground);
                window.draw(spriteActive1);
                window.draw(spriteActive2);
                window.draw(spriteSelect);

                //Show everything that has been drawn.
                window.display();
            }
        }
    }
}
void moveTreeBranch(Sprite& part, int endGoal, bool& activeFall)
{
    float fallSpeed = 150;
    if (part.getPosition().y < endGoal || activeFall == true)
    {
        part.setPosition(part.getPosition().x, part.getPosition().y + (fallSpeed * dt.asSeconds()));
        if (part.getPosition().y > 112)
        {
            activeFall = true;
            if (playerPos == false)
            {
                part.setPosition(part.getPosition().x + (fallSpeed * dt.asSeconds()), part.getPosition().y);
                part.setRotation(part.getRotation() + (2* fallSpeed * dt.asSeconds()));
            }
            else
            {
                part.setPosition(part.getPosition().x - (fallSpeed * dt.asSeconds()), part.getPosition().y);
                part.setRotation(part.getRotation() - (2* fallSpeed * dt.asSeconds()));
            }
        }
    }
}
void moveTreeTrunk(Sprite& part, int endGoal)
{
    float fallSpeed = 150;
    if (part.getPosition().y < endGoal)
    {
        part.setPosition(part.getPosition().x, part.getPosition().y + (fallSpeed * dt.asSeconds()));
        if (part.getPosition().y > 111)
        {
            if (playerPos == false)
            {
                part.setPosition(part.getPosition().x + (fallSpeed * dt.asSeconds()), part.getPosition().y);
                //part.setRotation(part.getRotation() + (2* fallSpeed * dt.asSeconds()));
            }
            else
            {
                part.setPosition(part.getPosition().x - (fallSpeed * dt.asSeconds()), part.getPosition().y);
                //part.setRotation(part.getRotation() - (2* fallSpeed * dt.asSeconds()));
            }
        }
    }
    else
    {
        part.setPosition(103, endGoal);
        if (part.getPosition().y > 111)
        {
            part.setPosition(103, -33);
            //part.setRotation(0);
        }
        treeMoveOnce++;
        if (treeMoveOnce == 11)
        {

            treeMoveOnce = 0;
            treeMove = false;
        }
    }
}
void spawnBranch(Sprite& part, bool firstTime, bool& whichBranch)
{
    int randomiser;
    if (firstTime == true)
    {
        part.setRotation(0);
        whichBranch = false;
        randomiser = rand() % 10 + 1;
        if (randomiser > 1)
        {
            part.setPosition(105, 93 - (branchSpawnPos * 18));
        }
        else
        {
            part.setPosition(103, 240);
        }
        randomiser = rand() % 2 + 1;
        if (randomiser == 2)
        {
            part.setScale(-1, 1);
        } else
        {
            part.setScale(1, 1);
            part.setPosition(134, part.getPosition().y);
        }
        branchSpawnPos = branchSpawnPos + 2;
    }
    else
    {
        if (spawnAnotherBranch == true)
        {
            randomiser = rand() % 10 + 1;
            if (randomiser > 2)
            {
                part.setRotation(0);
                whichBranch = false;
                spawnAnotherBranch = false;
                randomiser = rand() % 2 + 1;
                if (randomiser == 2)
                {
                    part.setPosition(105, -33);
                    part.setScale(-1, 1);
                }
                else
                {
                    part.setScale(1, 1);
                    part.setPosition(134, -33);
                }
            }
        } else
        {
            spawnAnotherBranch = true;
        }
    }
}
void collisionCheck(Sprite& branch, bool& playerDead)
{
    if (branch.getPosition().y <= 114 && branch.getPosition().y >= 108)
    {
        
        if (branch.getScale().x == 1 && playerPos == true)
        {
            playerDead = true;
        }
        else if (branch.getScale().x == -1 && playerPos == false)
        {
            playerDead = true;
        }
    }
}
void cloudMovement(struct cloudData& cloudD)
{
    int counter;
    for (counter = 0; counter < 3; counter++)
    {
        if (!cloudD.cloudActive[counter])
        {
            //How fast is the cloud.
            cloudD.cloudSpeed[counter] = (rand() % 8 + 2);
            //How high is the cloud.
            float height = (rand() % 100 + 20);
            cloudD.cloudSprites[counter].setPosition(-74, height);
            cloudD.cloudActive[counter] = true;
        }
        else
        {
            cloudD.cloudSprites[counter].setPosition(cloudD.cloudSprites[counter].getPosition().x
                + (cloudD.cloudSpeed[counter] * dt.asSeconds()), cloudD.cloudSprites[counter].getPosition().y);
            //Has cloud got to the end yet.
            if (cloudD.cloudSprites[counter].getPosition().x > 240)
            {
                //Reset the cloud.
                cloudD.cloudActive[counter] = false;
            }
        }
    }
}
void birdMovement(bool& birdActive, int& birdSpeed, Sprite& birdSprite, IntRect& birdSpriteRect, float& birdTimer)
{
    if (!birdActive)
    {
        //How fast is the cloud.
        birdSpeed = (rand() % 20 + 15);
        //How high is the bird.
        float height = (rand() % 100 + 20);
        birdSprite.setPosition(340, height);
        birdActive = true;
    }
    else
    {
        birdSprite.setPosition(birdSprite.getPosition().x
            - (birdSpeed * dt.asSeconds()), birdSprite.getPosition().y);
        //Has cloud got to the end yet.
        if (birdSprite.getPosition().x < -110)
        {
            //Reset the cloud.
            birdActive = false;
        }
    }
    if (birdTimer > 0.6)
    {
        birdTimer = 0;
    }
    else if (birdTimer > 0.5)
    {
        birdSpriteRect.left = 10;
    }
    else if (birdTimer > 0.4)
    {
        birdSpriteRect.left = 20;
    }
    else if (birdTimer > 0.3)
    {
        birdSpriteRect.left = 30;
    }
    else if (birdTimer > 0.2)
    {
        birdSpriteRect.left = 40;
    }
    else if (birdTimer > 0.1)
    {
        birdSpriteRect.left = 20;
    }
    else
    {
        birdSpriteRect.left = 0;
    }
}
void treeMovement(struct logData& logD, struct branchData branchD, bool& playerIsDead)
{
    int counter;
    if (treeMove == true)
    {
        if (treeMoveOnce == 1)
        {
            counter = 0;
            while (counter < 9)
            {
                logD.logGoals[counter] = logD.logSprites[counter].getPosition().y + 18;
                counter++;
            }
            for (counter = 0; counter < 4; counter++)
            {
                branchD.branchGoals[counter] = branchD.branchSprites[counter].getPosition().y + 18;
            }
            for (counter = 0; counter < 4; counter++)
            {
                if (branchD.branchFalls[counter] == true)
                {
                    spawnBranch(branchD.branchSprites[counter], false, branchD.branchFalls[counter]);
                    counter = 4;
                }
            }
            treeMoveOnce = 2;
        }
        counter = 0;
        while (counter < 9)
        {
            moveTreeTrunk(logD.logSprites[counter], logD.logGoals[counter]);
            counter++;
        }

        for (counter = 0; counter < 4; counter++)
        {
            moveTreeBranch(branchD.branchSprites[counter], branchD.branchGoals[counter],
                branchD.branchFalls[counter]);
        }
    }

    for (counter = 0; counter < 4; counter++)
    {
        if (branchD.branchFalls[counter] == true)
        {
            std::cout << " and the end goal is ";
            moveTreeBranch(branchD.branchSprites[counter], branchD.branchGoals[counter],
                branchD.branchFalls[counter]);
        }
        collisionCheck(branchD.branchSprites[counter], playerIsDead);
    }
}
void highScore(int score, std::fstream& myfile)
{
    int currentHighScore;
    std::string highScoreString;
    bool scoreHigher;

    myfile.open("highscore.txt");
    getline(myfile, highScoreString);
    myfile.close();

    currentHighScore = stoi(highScoreString);
    globalHighScore = currentHighScore;
    if(currentHighScore>score)
    {
        scoreHigher = false;
    }
    else
    {
        scoreHigher = true;
    }

    if(scoreHigher == true)
    {
        myfile.open("highscore.txt");
        myfile << score;
        myfile.close();
        globalHighScore = score;
    }
}