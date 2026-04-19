#include <iostream>
#include <cmath>

using namespace std;

class Point // Класс точка, хранит координаты и вектора
{
public:
	double x, y, Vx, Vy;

	void Update()
	{
		cin >> x >> y >> Vx >> Vy;
	}
};

class Chaser : public Point // Класс преследователя
{
public:
	double V, Angle = 0;

	void UpdateAngle(double PredictX, double PredictY)
	{
		Angle = atan2((PredictY - y), (PredictX - x)); // Функция, вычисляет угол в рад.
	}
};

class Target : public Point // Класс цели
{
public:
	double Distance = 0;
	bool IsChased = false;

	void Update(Chaser &chaser)
	{
		Point::Update(); 
		cin >> IsChased;

		double dx = x - chaser.x;
		double dy = y - chaser.y;
		Distance = sqrt((dx * dx + dy * dy)); // Расстояние от преследователя до цели, через гипотенузу
	}
};



int main()
{
	Chaser chaser;  // Преследователь
	double Time;
	int N, i;
	bool ChasedAll = false;

	cin >> chaser.V >> N;  // Вводим начальные данные
	Target* Targets = new Target[N]; // Массив целей



	while (!ChasedAll) // Начало моделирования
	{
		ChasedAll = true; 

		cin >> Time; // Обновляем все данные
		chaser.Update(); 
		Target* CurrentTarget = nullptr; // Указывает на ближайшую цель. Обнуляется, чтобы не указывать на пойманные цели в будущем
		for (i = 0; i < N; i++)
		{
			Targets[i].Update(chaser);
			if (!Targets[i].IsChased) // Если цель не поймана - смотрим является ли она ближайшей
			{
				ChasedAll = false;
				if (CurrentTarget == nullptr || CurrentTarget->Distance > Targets[i].Distance)
					CurrentTarget = &Targets[i];
			}
		}
		
		if (CurrentTarget != nullptr)
		{
			double PredictX = ((CurrentTarget->Distance / chaser.V) * CurrentTarget->Vx) + CurrentTarget->x; // Координаты, где встретятся цель и преследователь. По формуле:
			double PredictY = ((CurrentTarget->Distance / chaser.V) * CurrentTarget->Vy) + CurrentTarget->y; // ((Время до достижения цели преследователем) *  Скорость цели) + текущее положение цели
			chaser.UpdateAngle(PredictX, PredictY);
		}

		cout << chaser.Angle << endl; // Упр. команды
		cout << ChasedAll << endl;
	}
	
	delete[] Targets;

	return 0;
}