#include <iostream>
#include <raylib.h>
#include <deque> //poruszajacy sie waz
#include <raymath.h> //dodawanie wektorow przy poruszaniu

using namespace std;

double LastUpdateTime = 0;

int cells1 = 25; //25*30 = 750
int cells2 = 30;
int score = 0;

Color brightgreen = { 17, 227, 0, 255 };

void TerminatingProgram()
{
    exit(1);
};

//do porownywania czy glowa miala kolizje z ktoryms z elementow ciala
bool ElementInDeque(Vector2 element, deque<Vector2> body)
{
    for (unsigned int i = 0; i < body.size(); i++)
    {
        if (Vector2Equals(body[i], element))
        {
            return true;
        }
    }
    return false;
}

class Snake
{
private:
    Rectangle segment;

public:
    deque<Vector2> body = { Vector2{6, 9}, Vector2{5, 9}, Vector2{4, 9} };
    Vector2 direction = { 1, 0 };

    //rysowanie segmentów wê¿a
    void Draw()
    {
        for (unsigned int i = 0; i < body.size(); i++)
        {
            float x = body[i].x; //x w deque w wektorach
            float y = body[i].y;
            segment = Rectangle{ x * cells1, y * cells1, (float)cells1, (float)cells1 };

            DrawRectangleRounded(segment, 0.5, 6, DARKGREEN);
        }

    }
    
    //4 kierunki - strzalki
    void MovingSnake()
    {

        double CurrentTime = GetTime(); //GetTime - pobieranie czasu uruchomienia prog. z biblioteki raylib

        if (CurrentTime - LastUpdateTime >= 0.15) //stala predkosc, aktualizacje
        {

            body.pop_back();
            body.push_front(Vector2Add(body[0], direction));
            LastUpdateTime = CurrentTime;
        }

        if (IsKeyPressed(KEY_RIGHT))
        {
            direction = { 1, 0 };
            body.push_front(Vector2Add(body[0], direction));
            body.pop_back();
        }

        if (IsKeyPressed(KEY_LEFT))
        {
            direction = { -1, 0 };
            body.push_front(Vector2Add(body[0], direction));
            body.pop_back();
        }

        if (IsKeyPressed(KEY_UP))
        {
            direction = { 0, -1 };
            body.push_front(Vector2Add(body[0], direction));
            body.pop_back();
        }

        if (IsKeyPressed(KEY_DOWN))
        {
            direction = { 0, 1 };
            body.push_front(Vector2Add(body[0], direction));
            body.pop_back();
        }



    }

    Vector2 GetSnakeHead()
    {
        return body[0];
    }

    //kolizje z granicami
    void CollisionsWithBorders()
    {
        if (body[0].x == -1 || body[0].x == cells2 || body[0].y == -1 || body[0].y == cells2) 
        {
            cout << "collision";
           TerminatingProgram();
              
        }
    }

   /* kolizja z cialem*/
    void CollisionWithBody()
    {
        deque<Vector2> snakesBody = body;
        snakesBody.pop_front();

        if (ElementInDeque(body[0], snakesBody))
        {
            cout << "collision with body";
            TerminatingProgram();

        }
    }
};
//rusza sie w 4 kierunkach



class Food
{    
public:
    Texture2D texture; //tekstura
    Vector2 position;
    Rectangle foodRectangle;

   

    //konstrktor do dynamicznego zmieniania polozenia jedzenia
    Food()  
    {
        Image img = LoadImage("C:/Users/Robert/source/repos/mysnake/mysnake/foods/Foodready4.png"); //pobranie obrazka z plikow
        ImageResize(&img, 275, 35);
        texture = LoadTextureFromImage(img); //konwersja obrazka na teksture
        UnloadImage(img);
        position = GenerateRandomFood(); //zmienianie pozycji obrazka
        

        float frameWidth = (float)(texture.width / 7); //podzielenie obrazka na mniejsze czesci
        foodRectangle = { 0, 0, frameWidth, (float)texture.height };

    }

    ~Food()
    {
        UnloadTexture(texture);
    }

    // generowanie losowych wspó³rzêdnych x i y wektora position
    Vector2 GenerateRandomFood()
    {
        float x = GetRandomValue(0, cells1); //do mieszczenia sie wizualnie na mapie
        float y = GetRandomValue(0, cells1);
        return Vector2{ x, y };
        
    }

    //rysowanie obrazka na mapie
    void Draw() 
    {
        DrawTextureRec(
            texture,
            foodRectangle,
            Vector2{ (position.x * cells1), (position.y * cells1) }, //pozycja obrazka
            WHITE);
    }
    
    //kolizja g³owa weza z jedzeniem
    void Collision(class Snake& other)
    {
        Vector2 snake = other.GetSnakeHead();
        if (Vector2Equals(position, snake))
        {
            position = GenerateRandomFood();
            if (other.body[other.body.size()-1].x < other.body[other.body.size()-2].x)
            {
                other.body.push_back({other.body[other.body.size() - 2].x - 1, other.body[other.body.size() - 2].y });
            }
            else if (other.body[other.body.size() - 1].x < other.body[other.body.size() - 2].x)
            {
                other.body.push_back({ other.body[other.body.size() - 2].x + 1, other.body[other.body.size() - 2].y });
            }
            else if (other.body[other.body.size() - 1].y < other.body[other.body.size() - 2].y)
            {
                other.body.push_back({ other.body[other.body.size() - 2].x , other.body[other.body.size() - 2].y + 1 });
            }
            else if (other.body[other.body.size() - 1].y > other.body[other.body.size() - 2].y)
            {
                other.body.push_back({ other.body[other.body.size() - 2].x , other.body[other.body.size() - 2].y - 1 });
            }
            score++;
        }
    }

};
//pojawia sie i zmienia lokalizacje

int main()
{
    InitWindow(cells1 * cells2, cells2 * cells1, "snake");

    Food food;
    Snake snake;

    SetTargetFPS(60);

    while (!WindowShouldClose())
    {   
        BeginDrawing();
        ClearBackground(GREEN);

        DrawText(TextFormat("%i", score), 20, 20, 40, DARKPURPLE);
       
        snake.MovingSnake();
        snake.CollisionsWithBorders();
        snake.CollisionWithBody();
        food.Collision(snake);


        food.Draw();
        snake.Draw();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}

