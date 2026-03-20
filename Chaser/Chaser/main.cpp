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
	double V, Angle;

	void UpdateAngle(double dx, double dy)
	{
		Angle = atan2(dy, dx); // Функция, вычисляет угол в рад.
	}
};

class Target : public Point // Класс цели
{
public:
	double Distance, dx, dy;
	bool IsChased = false;
	
	void Update(Chaser &chaser)
	{
		Point::Update(); 
		cin >> IsChased; // Поймана ли цель

		dx = x - chaser.x;
		dy = y - chaser.y;
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
		int ChasedCount = 0; // Счетчик пойманных. Если == N - поймали всех
		Target* CurrentTarget = nullptr; // Указывает на ближайшую цель. Обнуляется, чтобы не указывать на пойманные цели в будущем 

		cin >> Time; // Обновляем все данные
		chaser.Update();  
		for (i = 0; i < N; i++)
		{
			Targets[i].Update(chaser);
			if (!Targets[i].IsChased) // Если цель не поймана - смотрим является ли она ближайшей
			{
				if (CurrentTarget == nullptr || CurrentTarget->Distance > Targets[i].Distance)
					CurrentTarget = &Targets[i];
			}
			else
				ChasedCount++; 
		}
		
		if(CurrentTarget != nullptr)
			chaser.UpdateAngle(CurrentTarget->dx, CurrentTarget->dy);
		if (ChasedCount == N) // Проверка поймали ли всех
			ChasedAll = true;

		cout << chaser.Angle << endl; // Упр. команды
		cout << ChasedAll << endl;
	}
	
	delete[] Targets;

	return 0;
}