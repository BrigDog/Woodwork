//Include libaries here.
#include <sstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <list>
#include<string>  
#include<cmath>
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

sf::Vector2f round(const sf::Vector2f vector)
{
    return sf::Vector2f{ std::round(vector.x), std::round(vector.y) };
}

//This is where the game starts from.
int main()
{
    //All local variables stored here in sections of their data type.
    struct logData logD;
    struct branchData branchD;
    struct cloudData cloudD;
    struct playerData playerD;

    void moveTreeTrunk(Sprite&, int);
    void spawnBranch(Sprite&, bool, bool&);
    void moveTreeBranch(Sprite&, int, bool&);
    void collisionCheck(Sprite&, bool&);
    void cloudMovement(cloudData&);
    void treeMovement(struct logData&, struct branchData, bool&);
    void birdMovement(bool&, int&, Sprite&, IntRect&, float&);

    Clock clock;

    bool playerIsDead = false;
    bool playerHit = false;
    bool paused = true;

    int counter = 0;
    int score = 0;
    int characterPicker = 0;

    float playerTime = 0;
    float waitTime = 0;

    //Making text and loading font.
    sf::Font font;
    font.loadFromFile("fonts/space-harrier-extended.ttf");
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setPosition(116, 0);
    scoreText.setLetterSpacing(0);
    scoreText.setLineSpacing(0);
    scoreText.setCharacterSize(80);
    scoreText.setScale(0.1f, 0.1f);
    scoreText.setFillColor(sf::Color::Black);
    scoreText.setString(std::to_string(score));

    //Create a video mode object.
    VideoMode vm(240, 160);

    //Create and open a window for the game.
    RenderWindow window(vm, "Woodwork", Style::Default);

    //Create a log sprite.
    logD.logTexture.loadFromFile("graphics/treePart.png");
    counter = 0;
    while (counter < 9)
    {
        logD.logSprites[counter].setTexture(logD.logTexture);
        logD.logSprites[counter].setPosition(103, 111 - (18* counter));
        counter++;
    }

    //Create a branch sprite.
    branchD.branchTexture.loadFromFile("graphics/branch.png");
    for(counter = 0; counter < 4; counter++)
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
    playerD.playerTextures[0].loadFromFile("graphics/defaultPlayer1.png");
    playerD.playerTextures[1].loadFromFile("graphics/defaultPlayer2.png");
    playerD.playerTextures[2].loadFromFile("graphics/defaultPlayer3.png");
    playerD.playerSprite.setTexture(playerD.playerTextures[0]);
    playerD.playerSprite.setPosition(65, 100);

    //Create a bird sprite
    Texture birdTexture;
    birdTexture.loadFromFile("graphics/birdSpritesheet.png");
    sf::IntRect birdSpriteRect(0, 0, 10, 14);
    sf::Sprite birdSprite(birdTexture, birdSpriteRect);
    birdSprite.setPosition(240, 0);
    bool birdActive = false;
    int birdSpeed = 5;
    float birdTimer = 0;

    //Make a chop sound.
    sf::SoundBuffer chop;
    // load something into the sound buffer...
    chop.loadFromFile("audio/chop.wav");
    sf::Sound sound;
    sound.setBuffer(chop);

    //Make a background.
    //Create a texture to hold a graphic on the GPU.
    Texture textureBackground;
    //Load a graphic into the texture.
    textureBackground.loadFromFile("graphics/background.png");
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
    textureStump.loadFromFile("graphics/stump.png");
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
    textureMidground.loadFromFile("graphics/midground.png");
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
    textureGravestone.loadFromFile("graphics/gravestone.png");
    //Create a sprite
    Sprite spriteGravestone;
    //Attach the texture to the sprite.
    spriteGravestone.setTexture(textureGravestone);
    //Set the position of the sprite.
    spriteGravestone.setPosition(73, 121);

    //Begins a while loop to stop the program from ending.
    while (window.isOpen()) 
    {
        //Handle the application resize and close functionality
        sf::Event _event;
        while (window.pollEvent(_event)) 
        {
            if (_event.type == sf::Event::Closed) {
                window.close();
            }
        }

        //Handle the player input.
        if (Keyboard::isKeyPressed(Keyboard::Q)) 
        {
            window.close();
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
                playerD.playerSprite.setPosition(174, 100);
                playerD.playerSprite.setScale(-1, 1);
                spriteGravestone.setPosition(143, 121);
            }

            if (Keyboard::isKeyPressed(Keyboard::A))
            {
                playerPos = false;
                playerD.playerSprite.setPosition(65, 100);
                playerD.playerSprite.setScale(1, 1);
                spriteGravestone.setPosition(73, 121);
            }

            if (Keyboard::isKeyPressed(Keyboard::S))
            {
                playerIsDead = true;
            }

            if (Keyboard::isKeyPressed(Keyboard::X))
            {
                std::cout << branchD.branchSprites[0].getPosition().y;
            }
        }
        
        if (Keyboard::isKeyPressed(Keyboard::R)&& reset == true)
        {
            reset = false;
            playerIsDead = false;
            playerHit = false;
            paused = true;
            counter = 0;
            score = 0;
            scoreText.setString(std::to_string(score));
            scoreText.setPosition(116, 0);
            playerTime = 0;
            treeMove = false;
            treeMoveOnce = 0;
            branchSpawnPos = 0;
            spawnAnotherBranch = true;
            for (counter = 0; counter < 4; counter++)
            {
                spawnBranch(branchD.branchSprites[counter], true, branchD.branchFalls[counter]);
            }
        }
        if (Keyboard::isKeyPressed(Keyboard::C) && reset == true)
        {
            //Make a timer so player can't spam.
            reset = false;
            //Pick which character.
            characterPicker++;
            if (characterPicker >= 2)
            {
                characterPicker = 0;
            }
            //Change character textures.
            if (characterPicker == 0)
            {
                playerD.playerTextures[0].loadFromFile("graphics/defaultPlayer1.png");
                playerD.playerTextures[1].loadFromFile("graphics/defaultPlayer2.png");
                playerD.playerTextures[2].loadFromFile("graphics/defaultPlayer3.png");
            } else if (characterPicker == 1)
            {
                playerD.playerTextures[0].loadFromFile("graphics/winterPlayer1.png");
                playerD.playerTextures[1].loadFromFile("graphics/winterPlayer2.png");
                playerD.playerTextures[2].loadFromFile("graphics/winterPlayer3.png");
            }
            //reset the player sprite;
            playerD.playerSprite.setTexture(playerD.playerTextures[1]);
        }

        //Start the game
        if (Keyboard::isKeyPressed(Keyboard::Escape)) 
        {
            dt = clock.restart();
            paused = !paused;
        }

        //Update the scene.
        if (reset == false)
        {
            waitTime += dt.asSeconds();
            if (waitTime >= 5)
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
            //Manage the tree
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
                    score++;
                    scoreText.setString(std::to_string(score));
                    if (score >= 100)
                    {
                        scoreText.setPosition(108, 0);
                    }
                    else if(score >= 10)
                    {
                        scoreText.setPosition(112, 0);
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
        window.draw(birdSprite);
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
        if(playerIsDead == false)
        {
            window.draw(playerD.playerSprite);
            //window.draw(spritePlayer);
        }
        else
        {
            window.draw(spriteGravestone);
        }
        window.draw(scoreText);
        

        //Show everything that has been drawn.
        window.display();
    }
    //Ends the program.
    return 0;
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
