#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
#include <vector>
#include <random>
#include <execution>
#include <thread>
#define _USE_MATH_DEFINES

// Preferences and prepaconstrations

const int windowWidth = 1920, windowHeight = 1080;
const int AGENTS_NUMBER = 50000;
const int AGENT_SPEED = 1;
const int SENSOR_LENGTH = 6; // MUST be greater than SENSOR_SIZE
const int SENSOR_SIZE = 2; //MUST be even
const double SENSOR_ANGLE = M_PI/4;
const int EDGE_L = windowHeight*0.01;
const double TURN_ANGLE = M_PI/8;
const float TURN_RAND = 0;
const float EVAPO_RATE = 0.997;

const sf::Color COLOR_WHITE(255,255,255);
const sf::Color COLOR_BLUE(1,1,255);
const sf::Color COLOR_RED(255,1,1);
const sf::Color COLOR_GREEN(1,255,1);

// const sf::Color COLOR_BLUE(76,168,248);
// const sf::Color COLOR_RED(239,71,111);
// const sf::Color COLOR_GREEN(255,209,102);

static std::default_random_engine e;
static std::uniform_real_distribution<> dis(0, 1);

float randomValue(float min = -1, float max = 1, bool allowMiddle = false );

void setPixels(sf::Image& image, std::vector<std::vector<int>>& buffer, sf::Color color);

void evaporateImage(sf::Image& img);

sf::Color getPixelColor(sf::Image& image, int x, int y);

float getAvAreaValue(sf::Image& image, int x, int y, int a,  sf::Color favColor = COLOR_WHITE);


class Agent 
{
    public:
    float x, y, vx, vy;
    int bcounter = 0;

    Agent()
    {
        x = 0;
        y = 0;
        vx = 0;
        vy = 0;
    }
    Agent(float initial_x, float initial_y, float initial_vx, float initial_vy)
    {
        x = initial_x;
        y = initial_y;
        vx = initial_vx;
        vy = initial_vy;
    }

    void move(int windowWidth, int windowHeight)
    {
        if (x > windowWidth - EDGE_L)
        {
            x = windowWidth - EDGE_L - 10;
            vx -= vx;
        }
        if  (x < EDGE_L)
        {
            x = EDGE_L + 10;
            vx -= vx;
        }
        if (y > windowHeight - EDGE_L)
        {
            y = windowHeight - EDGE_L - 10;
            vy -= vy;
        }
        if  (y < EDGE_L)
        {
            y = EDGE_L + 1;
            vy -= vy;
        }
        
        if (sqrt(pow(vx,2) + pow(vy,2)) <= 0.5)
        {
            vx = randomValue();
            vy = randomValue();
        }
        else
        {
            float pvx = vx, pvy = vy;
        int rsd = randomValue()*TURN_RAND;

        vx = pvx*cos(rsd*TURN_ANGLE) - pvy*sin(rsd*TURN_ANGLE);
        vy = pvy*cos(rsd*TURN_ANGLE) + pvx*sin(rsd*TURN_ANGLE);
        }

        x += vx;
        y += vy;
    }

    void search(sf::Image& image, sf::Color clr)
    {
        int lx, ly, srx, sry, slx, sly;
        float cvl, cvr;

        lx = SENSOR_LENGTH * vx / sqrt(pow(vx, 2) + pow(vy, 2));
        ly = SENSOR_LENGTH * vy / sqrt(pow(vx, 2) + pow(vy, 2));

        slx = int(x + lx*cos(SENSOR_ANGLE) - ly*sin(SENSOR_ANGLE));
        sly = int(y + ly*cos(SENSOR_ANGLE) + lx*sin(SENSOR_ANGLE));

        srx = int(x + lx*cos(-SENSOR_ANGLE) - ly*sin(-SENSOR_ANGLE));
        sry = int(y + ly*cos(-SENSOR_ANGLE) + lx*sin(-SENSOR_ANGLE));

        cvl = getAvAreaValue(image, slx, sly, SENSOR_SIZE, clr);
        cvr = getAvAreaValue(image, srx, sry, SENSOR_SIZE, clr);

        float pvx = vx, pvy = vy;

        if ((cvl == cvr) && (cvl*cvr !=0))
        {
            
            int rsd = randomValue();

            vx = pvx*cos(rsd*TURN_ANGLE) - pvy*sin(rsd*TURN_ANGLE);
            vy = pvy*cos(rsd*TURN_ANGLE) + pvx*sin(rsd*TURN_ANGLE);
        }
        else
        { if (cvl < cvr )
            {
                vx = pvx*cos(-TURN_ANGLE) - pvy*sin(-TURN_ANGLE);
                vy = pvy*cos(-TURN_ANGLE) + pvx*sin(-TURN_ANGLE);
            }
        else
        {   if (cvl > cvr )
        {
                vx = (pvx*cos(TURN_ANGLE) - pvy*sin(TURN_ANGLE));
                vy = (pvy*cos(TURN_ANGLE) + pvx*sin(TURN_ANGLE));
            }   
        }
        }
        
    }

};

class Swarm
{
    public:
    int x, y;
    int agentCounter;
    int size;
    std::vector<Agent> agents;
    sf::Color swarmColor;

    Swarm()
    {
        x = windowWidth/2;
        y = windowHeight/2; 
        agentCounter = 0;
    }

    Swarm(int ix,int iy, int agNum, sf::Color color, int sz = windowHeight*0.1)
    {
        x = ix;
        y = iy;
        agentCounter = agNum;
        swarmColor = color;
        size = sz;

        for (int i =0; i < agentCounter; i++)
        {
            agents.push_back(Agent(x + randomValue(0,1,true)*sin(2*M_PI*i/agentCounter)*size/2,y + randomValue(0,1,true)*cos(2*M_PI*i/agentCounter)*size/2, AGENT_SPEED*randomValue(), AGENT_SPEED*randomValue()));
        }

    }

    void act(sf::Image& img)
    {
        std::vector<std::vector<int>> PixelBuffer;
        std::for_each(std::execution::par, agents.begin(), agents.end(), [&](Agent& agent)
        {
            agent.search(img,swarmColor);
            agent.move(windowWidth,windowHeight);
            PixelBuffer.push_back({((int) agent.x), ((int) agent.y)});
        });

        setPixels(img, PixelBuffer, swarmColor);

    }
    
};

int main()
{
    int itercounter = 0;

    sf::RenderWindow window(sf::VideoMode(windowWidth, windowHeight), "ITS ALIIIIVE!"); //sf::Style::Fullscreen
    std::vector<Agent> agents;
    
    sf::Image MImage;
    MImage.create(windowWidth, windowHeight);

    Swarm Swarm_1 = Swarm(windowWidth/2, windowHeight/2, AGENTS_NUMBER/2, COLOR_GREEN, windowHeight/6);
    Swarm Swarm_2 = Swarm(windowWidth/2, windowHeight/2, AGENTS_NUMBER/2, COLOR_BLUE, windowHeight/4);
    Swarm Swarm_3 = Swarm(windowWidth/2, windowHeight/2, AGENTS_NUMBER/2, COLOR_RED, windowHeight/2 - 50);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
        }

        window.clear();
        evaporateImage(MImage);
        
        std::thread S1(&Swarm::act, &Swarm_1, std::ref(MImage));
        std::thread S2(&Swarm::act, &Swarm_2, std::ref(MImage));
        std::thread S3(&Swarm::act, &Swarm_3, std::ref(MImage));
        S1.join();
        S2.join();
        S3.join();
        
        sf::Texture MTexture;
        MTexture.loadFromImage(MImage); 
        sf::Sprite sprite(MTexture);
        window.draw(sprite);
        window.display();
        itercounter++;
    }

    return 0;
}

float randomValue(float min, float max, bool allowMiddle)
{
    float rcoef = dis(e);

    if (!allowMiddle)
    {
        while (rcoef == 0.5)
        {
            rcoef = dis(e);
        }
    }

    return rcoef*(max - min) + min;
}

void setPixels(sf::Image& image, std::vector<std::vector<int>>& buffer, sf::Color color){
    std::for_each(std::execution::par, buffer.begin(), buffer.end(), [&](auto& pixl)
        {
            if ((pixl.at(0) < image.getSize().x) && (pixl.at(0) > 0) && (pixl.at(1) < image.getSize().y) && (pixl.at(1) > 0))
            {
                image.setPixel(pixl.at(0), pixl.at(1), color);
            }
        });

    buffer.clear();
} 

void evaporateImage(sf::Image& img){
    for (int i = 1; i < img.getSize().x - 1; i++)
    {
        for (int j = 1; j < img.getSize().y - 1; j++)
        {
            sf::Color c = img.getPixel(i,j);
            if (c != sf::Color(0,0,0))
            {
                c = sf::Color(int(c.r*EVAPO_RATE),int(c.g*EVAPO_RATE),int(c.b*EVAPO_RATE));
                img.setPixel(i,j,c);
            }
        }
    }
}

sf::Color getPixelColor(sf::Image& image, int x, int y){
    if ((x < image.getSize().x) && (x > 0) && (y < image.getSize().y) && y > 0)
    {
        return image.getPixel(x,y);
    }
    else
    {
        return sf::Color(0,0,0);
    }
}

float getAvAreaValue(sf::Image& image, int x, int y, int a, sf::Color favColor){
    int avg = 0;
    for (int i = -a/2; i <= a/2; i++){
        for (int j = -a/2; j <= a/2; j++){
            sf::Color colr = getPixelColor(image, x + i, y + j);
            if (colr.r + colr.g + colr.b != 0)
            {
                if ((colr.r / favColor.r == colr.g / favColor.g) && (colr.r / favColor.r == colr.b / favColor.b) && (colr.g / favColor.g == colr.b / favColor.b))
            {
                avg += 1;
            }
            else
            {
                avg -= 1;
            }
            }
        }
    }
    return avg;
}