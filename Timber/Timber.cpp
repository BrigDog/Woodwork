//Include libaries here.
#include <sstream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include <list>
#include <string>  
#include <cmath>
#include <sqlite3.h>
#include <stdio.h>

//Make code easier to write with "using namespace"
using namespace sf;
using namespace std;

int globalHighScore = 0;

//All the structure data sets are made here.
struct logData
{
    Texture logTexture;
    Sprite logSprites[9];
    int logGoals[9];
};
struct branchData
{
    Texture branchTexture;
    Sprite branchSprites[4];
    int branchGoals[4];
    bool branchFalls[4];
};
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
struct soundData
{
    sf::SoundBuffer differentSFX[7];
    sf::Sound SFXSounds[7];
    sf::Music musicSounds[3];
};

string Score = "20";

static int createDB(const char* s);
static int createTable(const char* s);
static int deleteData(const char* s);
static int insertData(const char* s, int score);
static int updateData(const char* s);
static int selectData(const char* s, int& newScore);
static int callback(void* NotUsed, int argc, char** argv, char** azColName);

sf::Vector2f round(const sf::Vector2f vector)
{
    return sf::Vector2f{ std::round(vector.x), std::round(vector.y) };
}

//This is where the game starts from.
int main()
{
    const char* dir = R"(c:\Database\HIGHSCORES.db)";
    //Create a video mode object.
    VideoMode vm(240, 160);

    //Create and open a window for the game.
    RenderWindow window(vm, "Woodwork", Style::Default);

    //Making an instance of each structure and a name to go along with it.
    struct soundData soundD;
    struct logData logD;
    struct branchData branchD;
    struct cloudData cloudD;
    struct cloudData menuCloudD;
    struct playerData playerD;
    struct playerData menuPlayerD;
    struct timeBarData timeBarD;

    //Initializing all the void methods.
    void cloudMovement(cloudData&, Time);
    void birdMovement(bool&, int&, Sprite&, IntRect&, float&, Time);
    void textSettingsMenu(Text&, int xCord, int yCord, String textString, sf::Font&);

    //All variables used throughout the game.
    std::fstream myfile;

    Clock clock;

    int characterPicker = 0;
    int mapPicker = 0;
    int treeMoveOnce = 0;
    int branchSpawnPos = 0;

    Time dt;

    bool treeMove = false;   
    bool playerPos = false;
    bool spawnAnotherBranch = true;
    bool reset = true;

    //Creating the inital gamestate.
    String gameState = "Menu";

    //Creating all the sounds and music of the game.
    soundD.musicSounds[0].openFromFile("audio/Music/Menu Music - RoccoW - Welcome!.wav");
    soundD.musicSounds[1].openFromFile("audio/Music/Summer Music - RoccoW - Bleeps Galore.wav");
    soundD.musicSounds[2].openFromFile("audio/Music/Winter Music - RoccoW - Fuck Sidechain Compression on Gameboy.wav");

    soundD.differentSFX[0].loadFromFile("audio/SFX/Chop - 270325__littlerobotsoundfactory__hit-02.wav");
    soundD.SFXSounds[0].setBuffer(soundD.differentSFX[0]);
    soundD.differentSFX[1].loadFromFile("audio/SFX/Milestone Achievement - 270331__littlerobotsoundfactory__jingle-achievement-00.wav");
    soundD.SFXSounds[1].setBuffer(soundD.differentSFX[1]);
    soundD.differentSFX[2].loadFromFile("audio/SFX/New Highscore - 270333__littlerobotsoundfactory__jingle-win-00.wav");
    soundD.SFXSounds[2].setBuffer(soundD.differentSFX[2]);
    soundD.differentSFX[3].loadFromFile("audio/SFX/Player Move - 270316__littlerobotsoundfactory__open-00.wav");
    soundD.SFXSounds[3].setBuffer(soundD.differentSFX[3]);
    soundD.differentSFX[4].loadFromFile("audio/SFX/Menu Switch -270324__littlerobotsoundfactory__menu-navigate-00.wav");
    soundD.SFXSounds[4].setBuffer(soundD.differentSFX[4]);
    soundD.differentSFX[5].loadFromFile("audio/SFX/Menu Enter - 270309__littlerobotsoundfactory__craft-00.wav");
    soundD.SFXSounds[5].setBuffer(soundD.differentSFX[5]);
    soundD.differentSFX[6].loadFromFile("audio/SFX/Level Lost - 270329__littlerobotsoundfactory__jingle-lose-00.wav");
    soundD.SFXSounds[6].setBuffer(soundD.differentSFX[6]);
    while (window.isOpen())
    {
        if (gameState == "Menu")
        {
            //All the variables used in the main menu state.
            int selectPosition = 50;
            int counter = 0;
            int menuBirdSpeed = 5;

            float resetTime = 0;
            float menuBirdTimer = 0;

            bool menuBirdActive = false;
            bool cloud1Active;

            //Resumes the menu music and pauses the game music.
            soundD.musicSounds[0].pause();
            soundD.musicSounds[1].pause();
            soundD.musicSounds[2].pause();
            soundD.musicSounds[0].play();
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
            //Checks the map selected and displays that on the menu.
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
                cloud1Active = true;
                menuCloudD.cloudActive[counter] = false;
                menuCloudD.cloudSprites[counter].setPosition(-74, 0);
            }
            counter = 0;

            //Creates all the text that appears on the screen.
            sf::Font font;
            font.loadFromFile("fonts/space-harrier-extended.ttf");

            sf::Text title;
            textSettingsMenu(title, 88, 5, "WOODWORK", font);

            sf::Text play;
            textSettingsMenu(play, 88, 50, "PLAY", font);

            sf::Text customize;
            textSettingsMenu(customize, 88, 70, "CUSTOMIZE", font);

            sf::Text quit;
            textSettingsMenu(quit, 88, 90, "QUIT", font);

            //Checks the selected character and displays the character on the menu.
            if (characterPicker == 0) {

                menuPlayerD.playerTextures[0].loadFromFile("graphics/defaultPlayer1.png");
            }
            else {
                menuPlayerD.playerTextures[0].loadFromFile("graphics/winterPlayer1.png");
            }
            menuPlayerD.playerSprite.setTexture(menuPlayerD.playerTextures[0]);
            menuPlayerD.playerSprite.setPosition(30, 100);

            while (window.isOpen() & gameState == "Menu")
            {
                //Resets the time from how much time the last frame took.
                dt = clock.restart();

                //Allows the user the quit the game using the cross on the window.
                //Also allows the user to maximize the window.
                sf::Event _event;
                while (window.pollEvent(_event))
                {
                    if (_event.type == sf::Event::Closed) {
                        window.close();
                    }
                }

                //Manages all the keyboard inputs.
                //Allows the player to enter the different gamestates.
                if (Keyboard::isKeyPressed(Keyboard::Enter))
                {
                    if (reset == true)
                    {
                        reset = false;
                        soundD.SFXSounds[5].play();
                        switch (selectPosition) {
                        case 50:
                            gameState = "Game";
                            window.clear();
                            break;
                        case 70:
                            gameState = "Custom";
                            window.clear();
                            break;
                        default:
                            window.close();
                        }
                    }
                }

                //Allows the player to move the select icon downwards.
                if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S))
                {
                    if (reset == true)
                    {
                        soundD.SFXSounds[4].play();
                        reset = false;
                        selectPosition += 20;
                    }
                }

                //Allows the player to move the select icon upwards.
                if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W))
                {
                    if (reset == true)
                    {
                        soundD.SFXSounds[4].play();
                        reset = false;
                        selectPosition -= 20;
                    }
                }

                //Reset is used so the player cannot spam controls.
                if (reset == false)
                {
                    resetTime += dt.asSeconds();
                    if (resetTime >= 0.15f)
                    {
                        reset = true;
                        resetTime = 0.0f;
                    }
                }

                //Resets the UI of the select icon,
                //so if the player spams down it will reset to the top of the page.
                if (selectPosition > 90)
                {
                    selectPosition = 50;
                }
                else if (selectPosition < 50)
                {
                    selectPosition = 90;
                }

                //Manage the clouds and any other map specific animation.
                if (mapPicker == 0)
                {
                    menuBirdTimer += dt.asSeconds();
                    birdMovement(menuBirdActive, menuBirdSpeed, menuBirdSprite, menuBirdSpriteRect, menuBirdTimer, dt);
                }
                cloudMovement(menuCloudD, dt);

                //Sets the position of the current select UI.
                spriteMenuSelect.setPosition(0, selectPosition);

                //Clear everything from the last frame.
                window.clear();

                //Draws everything for this frame.
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
            void moveTreeTrunk(Sprite&, int, Time, bool&, int&, bool);
            void spawnBranch(Sprite&, bool, bool&, int&, bool&);
            void moveTreeBranch(Sprite&, int, bool&, Time, bool);
            void collisionCheck(Sprite&, bool&, bool);

            soundD.musicSounds[0].pause();
            soundD.musicSounds[1].pause();
            soundD.musicSounds[2].pause();
            if(mapPicker == 0)
            {
                soundD.musicSounds[1].play();
            } 
            if (mapPicker == 1)
            {
                soundD.musicSounds[2].play();
            }

            bool playerIsDead = false;
            bool playerHit = false;
            bool paused = true;
            bool scoreAnimation = false;

            int counter = 0;
            int score = 0;
            int lastScore = 0;

            float maxTime;
            if (mapPicker == 0)
            {
                maxTime = 1.2;
            } else
            {
                maxTime = 0.8;
            }
            float timeLeft = maxTime;
            float playerTime = 0;
            float scoreAnimationTime = 0;
            float waitTime = 0;

            selectData(dir, globalHighScore);

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
                spawnBranch(branchD.branchSprites[counter], true, branchD.branchFalls[counter], branchSpawnPos, spawnAnotherBranch);
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
                spawnBranch(branchD.branchSprites[counter], true, branchD.branchFalls[counter], branchSpawnPos, spawnAnotherBranch);
            }
            counter = 0;
            while (counter < 9)
            {
                logD.logSprites[counter].setTexture(logD.logTexture);
                logD.logSprites[counter].setPosition(103, 111 - (18 * counter));
                counter++;
            }

            reset = true;
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
                spawnBranch(branchD.branchSprites[counter], true, branchD.branchFalls[counter], branchSpawnPos, spawnAnotherBranch);
            }
            counter = 0;
            while (counter < 9)
            {
                logD.logSprites[counter].setTexture(logD.logTexture);
                logD.logSprites[counter].setPosition(103, 111 - (18 * counter));
                counter++;
            }

            bool resetYet = false;

            //Begins a while loop to stop the program from ending.
            while (window.isOpen() & gameState == "Game")
            {
                //Handle the application resize and close functionality
                sf::Event _event;
                while (window.pollEvent(_event))
                {
                    if (_event.type == sf::Event::Closed) {
                        if (score > globalHighScore)
                        {
                            globalHighScore = score;
                            deleteData(dir);
                            insertData(dir, score);
                        }
                        window.close();
                    }
                }

                //Handle the player input.
                if (Keyboard::isKeyPressed(Keyboard::Escape))
                {
                    if (soundD.SFXSounds[5].getStatus() == false)
                    {
                        soundD.SFXSounds[5].play();
                    }
                    if (score > globalHighScore)
                    {
                        globalHighScore = score;
                        deleteData(dir);
                        insertData(dir, score);
                    }
                    gameState = "Menu";
                    window.clear();
                }
                if (Keyboard::isKeyPressed(Keyboard::S))
                {
                    if (score > globalHighScore)
                    {
                        globalHighScore = score;
                        deleteData(dir);
                        insertData(dir, score);
                    }
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
                        if(soundD.SFXSounds[3].getStatus() == false)
                        {
                            soundD.SFXSounds[3].play();
                        }
                        playerPos = true;
                    }

                    if (Keyboard::isKeyPressed(Keyboard::A))
                    {
                        if (soundD.SFXSounds[3].getStatus() == false)
                        {
                            soundD.SFXSounds[3].play();
                        }
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
                    if (score > globalHighScore)
                    {
                        globalHighScore = score;
                        deleteData(dir);
                        insertData(dir, score);
                    }
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
                    resetYet = false;
                    for (counter = 0; counter < 4; counter++)
                    {
                        spawnBranch(branchD.branchSprites[counter], true, branchD.branchFalls[counter], branchSpawnPos, spawnAnotherBranch);
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
                if (Keyboard::isKeyPressed(Keyboard::Q) && reset == true && paused != false)
                {
                    reset = false;
                    dt = clock.restart();
                    paused = false;
                    if (soundD.SFXSounds[5].getStatus() == false)
                    {
                        soundD.SFXSounds[5].play();
                    }
                }
                if (Keyboard::isKeyPressed(Keyboard::P) && reset == true && paused != true)
                {
                    reset = false;
                    dt = clock.restart();
                    paused = true;
                    if (soundD.SFXSounds[5].getStatus() == false)
                    {
                        soundD.SFXSounds[5].play();
                    }
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
                                soundD.SFXSounds[0].play();
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
                            if(soundD.SFXSounds[1].getStatus() == false)
                            {
                                soundD.SFXSounds[1].play();
                            }
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
                                    spawnBranch(branchD.branchSprites[counter], false, branchD.branchFalls[counter], branchSpawnPos, spawnAnotherBranch);
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
                            moveTreeTrunk(logD.logSprites[counter], logD.logGoals[counter], dt, treeMove, treeMoveOnce, playerPos);
                            counter++;
                        }

                        for (counter = 0; counter < 4; counter++)
                        {
                            moveTreeBranch(branchD.branchSprites[counter], branchD.branchGoals[counter],
                                branchD.branchFalls[counter], dt, playerPos);
                        }
                    }

                    for (counter = 0; counter < 4; counter++)
                    {
                        if (branchD.branchFalls[counter] == true)
                        {
                            moveTreeBranch(branchD.branchSprites[counter], branchD.branchGoals[counter],
                                branchD.branchFalls[counter], dt, playerPos);
                        }
                        collisionCheck(branchD.branchSprites[counter], playerIsDead, playerPos);
                    }
                    //treeMovement(logD, branchD, playerIsDead);
                    //Manage the clouds.
                    cloudMovement(cloudD, dt);
                    //Manage the bird.
                    birdTimer += dt.asSeconds();
                    birdMovement(birdActive, birdSpeed, birdSprite, birdSpriteRect, birdTimer, dt);
                }
                if (playerIsDead == true)
                {
                    if (soundD.SFXSounds[6].getStatus() == false && resetYet == false)
                    {
                        resetYet = true;
                        soundD.SFXSounds[6].play();
                    }
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
            //Check to see if the player has made a highscore before.
            selectData(dir, globalHighScore);

            //Plays the correct music.
            soundD.musicSounds[0].pause();
            soundD.musicSounds[1].pause();
            soundD.musicSounds[2].pause();
            soundD.musicSounds[0].play();
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

            //Make a locked character icon.
            //Create a texture to hold a graphic on the GPU.
            Texture textureLockedChar;
            //Load a graphic into the texture.
            textureLockedChar.loadFromFile("graphics/winterCharacterLocked.png");
            //Create a sprite
            Sprite spriteLockedChar;
            //Attach the texture to the sprite.
            spriteLockedChar.setTexture(textureLockedChar);
            //Set the position of the sprite.
            spriteLockedChar.setPosition(144, 24);

            //Make a locked character icon.
            //Create a texture to hold a graphic on the GPU.
            Texture textureLockedMap;
            //Load a graphic into the texture.
            textureLockedMap.loadFromFile("graphics/winterMapLocked.png");
            //Create a sprite
            Sprite spriteLockedMap;
            //Attach the texture to the sprite.
            spriteLockedMap.setTexture(textureLockedMap);
            //Set the position of the sprite.
            spriteLockedMap.setPosition(52, 91);

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

            //All the varibles used in the customiztion menu.
            float resetTime = 0;

            int selectPositionY = 24;
            int selectPositionX = 52;

            bool charUnlocked = false;
            bool mapUnlocked = false;

            //Game state.
            while (window.isOpen() & gameState == "Custom")
            {
                //Using the highscore we loaded from the start.
                //Checks for any score above 150 or 250.
                //Unlocks the character and / or map depending on highscore.
                if(globalHighScore >= 150)
                {
                    charUnlocked = true;
                if (globalHighScore >= 250)
                {
                    mapUnlocked = true;
                }
                }

                //Reset the time, makes sure intereactions with keyboard are not to quick.
                if (reset == false)
                {
                    resetTime += dt.asSeconds();
                    if (resetTime >= 0.2f)
                    {
                        reset = true;
                        resetTime = 0.0f;
                    }
                }

                //Allows us to use the minimize and exit game button on top right of the window.
                sf::Event _event;
                while (window.pollEvent(_event))
                {
                    if (_event.type == sf::Event::Closed) {
                        window.close();
                    }
                }

                //Allows the user to change the selected icon based on which key is pressed.
                if (Keyboard::isKeyPressed(Keyboard::Down) || Keyboard::isKeyPressed(Keyboard::S))
                {
                    if (reset == true)
                    {
                        soundD.SFXSounds[4].play();
                        reset = false;
                        selectPositionY += 67;
                    }
                }
                if (Keyboard::isKeyPressed(Keyboard::Up) || Keyboard::isKeyPressed(Keyboard::W))
                {
                    if (reset == true)
                    {
                        soundD.SFXSounds[4].play();
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
                        soundD.SFXSounds[4].play();
                        reset = false;
                        selectPositionX += 92;
                    }
                }
                if (Keyboard::isKeyPressed(Keyboard::Left) || Keyboard::isKeyPressed(Keyboard::A))
                {
                    if (reset == true)
                    {
                        soundD.SFXSounds[4].play();
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

                //Allows us to select the current icon the player is hovering over on.
                if (Keyboard::isKeyPressed(Keyboard::Enter))
                {
                    if (reset == true)
                    {
                        soundD.SFXSounds[5].play();
                        reset = false;
                        if (selectPositionY == 24 & selectPositionX == 52)
                        {
                            spriteActive1.setPosition(52, 24);
                            characterPicker = 0;
                        }
                        if (selectPositionY == 24 & selectPositionX == 144)
                        {
                            if(charUnlocked == true)
                            {
                                spriteActive1.setPosition(144, 24);
                                characterPicker = 1;
                            }
                        }
                        if (selectPositionY == 91 & selectPositionX == 52)
                        {
                            if(mapUnlocked == true)
                            {
                                spriteActive2.setPosition(52, 91);
                                mapPicker = 1;
                            }
                        }
                        if (selectPositionY == 91 & selectPositionX == 144)
                        {
                            spriteActive2.setPosition(144, 91);
                            mapPicker = 0;
                        }
                    }
                    
                }

                //Allows us to exit back to main menu.
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

                //Draws everything to the current frame.
                window.draw(spriteBackground);
                window.draw(spriteMidground);
                if(charUnlocked == false)
                {
                    window.draw(spriteLockedChar);
                }
                if (mapUnlocked == false)
                {
                    window.draw(spriteLockedMap);
                }
                window.draw(spriteActive1);
                window.draw(spriteActive2);
                window.draw(spriteSelect);

                //Show everything that has been drawn.
                window.display();
            }
        }
    }
}
void moveTreeBranch(Sprite& part, int endGoal, bool& activeFall, Time dt, bool playerPos)
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
void moveTreeTrunk(Sprite& part, int endGoal, Time dt, bool& treeMove, int& treeMoveOnce, bool playerPos)
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
void spawnBranch(Sprite& part, bool firstTime, bool& whichBranch, int& branchSpawnPos, bool& spawnAnotherBranch)
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
void collisionCheck(Sprite& branch, bool& playerDead, bool playerPos)
{
    //Checks if the branch is on the same y level as the character sprite.
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
void cloudMovement(struct cloudData& cloudD, Time dt)
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
void birdMovement(bool& birdActive, int& birdSpeed, Sprite& birdSprite, IntRect& birdSpriteRect, float& birdTimer, Time dt)
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
    //Allows me to animate the bird based on how much time has passed.
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
void textSettingsMenu(Text& inputText, int xCord, int yCord, String textString, sf::Font& font)
{
    //Allows me to make all the text the same throughout the game.
    inputText.setLetterSpacing(0);
    inputText.setLineSpacing(0);
    inputText.setCharacterSize(80);
    inputText.setScale(0.1f, 0.1f);
    inputText.setFillColor(sf::Color::Black);
    inputText.setPosition(xCord, yCord);
    inputText.setString(textString);
    inputText.setFont(font);
}
static int createDB(const char* s)
{
    sqlite3* DB;

    int exit = 0;
    exit = sqlite3_open(s, &DB);

    sqlite3_close(DB);

    return 0;
}
static int createTable(const char* s)
{
    sqlite3* DB;
    char* messageError;

    string sql = "CREATE TABLE IF NOT EXISTS HIGHSCORES("
        "ID INTEGER PRIMARY KEY AUTOINCREMENT, "
        "USERNAME      TEXT NOT NULL, "
        "HIGHSCORE       INT  NOT NULL);";

    try
    {
        int exit = 0;
        exit = sqlite3_open(s, &DB);
        /* An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here */
        exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
        if (exit != SQLITE_OK) {
            cerr << "Error in createTable function." << endl;
            sqlite3_free(messageError);
        }
        else
            cout << "Table created Successfully" << endl;
        sqlite3_close(DB);
    }
    catch (const exception& e)
    {
        cerr << e.what();
    }

    return 0;
}
static int insertData(const char* s, int score)
{
    int newScore = 0;
    newScore = score;
    string Username = "User";
    sqlite3* DB;
    char* messageError;

    string sql(
        "INSERT INTO HIGHSCORES (USERNAME, HIGHSCORE) VALUES('" + Username + "', '" + std::to_string(newScore) + "');");

    int exit = sqlite3_open(s, &DB);

    /* An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here */
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        cerr << "Error in insertData function." << endl;
        sqlite3_free(messageError);
    }
    else
        cout << "Records inserted Successfully!" << endl;

    return 0;
}
static int updateData(const char* s)
{
    sqlite3* DB;
    char* messageError;

    string sql("UPDATE HIGHSCORES SET HIGHSCORE = 50 WHERE USERNAME = 'Josh'");

    int exit = sqlite3_open(s, &DB);
    /* An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here */
    exit = sqlite3_exec(DB, sql.c_str(), NULL, 0, &messageError);
    if (exit != SQLITE_OK) {
        cerr << "Error in updateData function." << endl;
        sqlite3_free(messageError);
    }
    else
        cout << "Records updated Successfully!" << endl;

    return 0;
}
static int deleteData(const char* s)
{
    sqlite3* DB;
    char* messageError;

    string sql = "DELETE FROM HIGHSCORES";

    int exit = sqlite3_open(s, &DB);
    /* An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here */
    exit = sqlite3_exec(DB, sql.c_str(), callback, NULL, &messageError);
    if (exit != SQLITE_OK) {
        cerr << "Error in deleteData function." << endl;
        sqlite3_free(messageError);
    }
    else
        cout << "Records deleted Successfully!" << endl;

    return 0;
}
static int selectData(const char* s, int& newScore)
{
    int value = 0;
    sqlite3* DB;
    char* messageError;

    string sql = "SELECT * FROM HIGHSCORES;";
    int exit = sqlite3_open(s, &DB);
    cout << exit;
    newScore = exit;



    /* An open database, SQL to be evaluated, Callback function, 1st argument to callback, Error msg written here*/
    exit = sqlite3_exec(DB, sql.c_str(), callback, NULL, &messageError);
    cout << exit;
    if (exit != SQLITE_OK) {
        if (exit != 4) {
            cerr << "Error in selectData function." << endl;
            sqlite3_free(messageError);
        }
        else {

            cout << "Records selected Successfully!" << endl;
        }
    }
    else {

        cout << "Records selected Successfully!" << endl;
    }
    return 0;
}

// retrieve contents of database used by selectData()
/* argc: holds the number of results, argv: holds each value in array, azColName: holds each column returned in array, */
static int callback(void* NotUsed, int argc, char** argv, char** azColName)
{
    string value;
    int newValue;
    for (int i = 0; i < argc; i++) {
        // column name and value
        cout << azColName[i] << ": " << argv[i] << endl;
        if (azColName[i] == azColName[2])
        {
            value += argv[i];
        }
    }
    cout << endl;
    newValue = stoi(value);
    globalHighScore = newValue;
    return newValue;
}