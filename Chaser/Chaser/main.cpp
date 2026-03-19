#include <iostream>
#include <cmath>

using namespace std;

class Point // Класс точка, хранит координаты, вектора и угол.
{
public:
	float x, y, Vx, Vy, V, Angle;

	void Update()
	{
		cin >> x >> y >> Vx >> Vy;
	}

	void UpdateAngle(float dx, float dy)
	{
		Angle = atan2(dy, dx); // Функция, вычисляет угол в рад.
	}
};

int main()
{
	Point Chaser;  // Преследователь
	float Time, status;
	int N, i;
	bool ChasedAll = false;
	

	cin >> Chaser.V >> N >> Time;  // Вводные данные
	Chaser.Update();               // Обновляем данные преследователя

	Point* Targets = new Point[N]; // На всякий случай массив целей, в будущем через него будет искаться ближайшая из множества целей
	for (i = 0; i < N; i++)        // Обновляем данные целей
		Targets[i].Update();
	Point* CurrentTarget = Targets; // Указатель на массив целей, будет указывать на ближайшую к преследователю

	float dx = CurrentTarget->x - Chaser.x; // Разница между координатами для поиска расстояния и угла
	float dy = CurrentTarget->y - Chaser.y;
	Chaser.UpdateAngle(dx, dy);
	float Distance = sqrt((dx * dx + dy * dy)); // Расстояние между целью и преследователем

	cout << Chaser.Angle << endl; // Упр. команды
	cout << ChasedAll << endl;


	while (!ChasedAll) // Начало моделирования
	{
		cin >> status; // Без понятия как объяснить. Во время моделирования выдает лишнее число 0. Специально делал код, который логировал все вводы - выводы. Либо оно возвращает моё же отправленное 0 (команду), либо что-то другое. Была теория, что это номер цели, но оно идёт перед временем
		cin >> Time;  // Обновляем все данные по всем точкам 
		Chaser.Update();  
		for (i = 0; i < N; i++)
			Targets[i].Update();
		
		dx = CurrentTarget->x - Chaser.x; // Обновляем угол, находим расстояние, можно потом в отдельную функцию вывести
		dy = CurrentTarget->y - Chaser.y;
		Chaser.UpdateAngle(dx, dy);
		Distance = sqrt((dx * dx + dy * dy));

		if (Distance <= 0.1) // Если достигли - поймали, флаг = true
			ChasedAll = true;


		cout << Chaser.Angle << endl; // Упр. команды
		cout << ChasedAll << endl;
	}
	
	CurrentTarget = nullptr;
	delete[] Targets;

	return 0;
}