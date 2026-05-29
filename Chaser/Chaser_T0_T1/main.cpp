#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>

using namespace std;

class Point // Базовый класс - точка, хранит координаты и вектора скорости точек
{
public:
	double Sx, Sy, Vx, Vy;
public:
	virtual void UpdateParameters()
	{
		cin >> Sx >> Sy >> Vx >> Vy;
	}
};

class Chaser : public Point // Дочерний класс - преследователь
{
public:
	double V; // Скорость преследователя
public:
	void SetSpeed()
	{
		cin >> V;
	}
};

class Target : public Point			// Дочерний класс - цель
{
public:
	bool IsChased;					// Поймана ли цель
	double ChaseTime, ChaseAngle;	// Время преследования цели и угол преследования
	double AimX, AimY;				// Координаты столкновения с преследователем

	static int CountTargets;		// Кол-во целей
	static bool ChasedAll;			// Пойманы ли все цели
public:
	void UpdateParameters()
	{
		Point::UpdateParameters();
		cin >> IsChased;
	}
	
	// Вычисление параметров (ChaseTime, ChaseAngle) для "охоты" на упреждение
	// т.е. преследователь не гонится за целью, а наперед просчитывает оптимальную точку столкновения
	// Для примера, уравение кинематики для оси Х: SxП + VxП * t = SxЦ + VxЦ * t
	// Но поскольку неизвестных два (скорость преследователя для каждоый оси и t), то преобразуем все в квадратное уравнение и находим t
	void CalculateAngle(double SxChaser, double SyChaser, double VChaser)
	{
		double Dx = Sx - SxChaser;
		double Dy = Sy - SyChaser;

		double A = (Vx * Vx + Vy * Vy) - (VChaser * VChaser);
		double B = 2 * (Dx * Vx + Dy * Vy);
		double C = Dx * Dx + Dy * Dy;

		double Discr = B * B - 4 * A * C;
		double sqrtD = sqrt(Discr);

		double t1 = (-B + sqrtD) / (2 * A);
		double t2 = (-B - sqrtD) / (2 * A);

		if (t1 > 0 && t2 > 0)
		{
			if (t1 < t2)
			{
				ChaseTime = t1;
			}
			else
			{
				ChaseTime = t2;
			}
		}
		else if (t1 > 0)
		{
			ChaseTime = t1;
		}
		else if (t2 > 0)
		{
			ChaseTime = t2;
		}
		else
		{
			ChaseTime = sqrt(Dx * Dx + Dy * Dy) / VChaser;
		}

		AimX = Sx + Vx * ChaseTime;								// Точка столкновения по оси Х
		AimY = Sy + Vy * ChaseTime;								// Точка столкновения по оси Y
		ChaseAngle = atan2(AimY - SyChaser, AimX - SxChaser);	// Вычисление угла для попадания в точку столкновения
	}
};
// Инициализация статических переменных
int Target::CountTargets = 0;
bool Target::ChasedAll = false;

int main()
{
	double Time;	// Время моделирования
	double TotalTime = 0; // Время поимки всех целей
	double BestTotalTime; // Кратчайшее время поимки всех целей, присвиваем 1 миллиард для корректного сравнения с временем первого полного пути
	double BufferXChaser, BufferYChaser; // Буферные координаты для запоминания положения преследователя
	double BufferXTarget, BufferYTarget; // Буферные координаты для запоминания положения целей
	int i;

	// Получение начальных данных
	Chaser* chaser = new Chaser;	// Преследователь
	chaser->SetSpeed();				// Получение скорости преследователю

	cin >> Target::CountTargets;						// Кол-во целей

	Target* Targets = new Target[Target::CountTargets]; // Массив целей
	vector <Target*> CombinationTargets;				// Массив указателей на цели для полного перебора через next_permutation из <algorithm>
	vector <Target*> BestCombinationTargets;

	// Начало моделирования
	while (!Target::ChasedAll)
	{
		// Обнуления
		Target::ChasedAll = true;
		CombinationTargets.clear();
		BestCombinationTargets.clear();
		BestTotalTime = 1e9;

		// Обновление параметров моделирования
		cin >> Time;
		chaser->UpdateParameters(); BufferXChaser = chaser->Sx; BufferYChaser = chaser->Sy;
		for (i = 0; i < Target::CountTargets; i++)
		{
			Targets[i].UpdateParameters();

		}

		// Заполнение массива для полного перебора непосещенных целей
		for (i = 0; i < Target::CountTargets; i++)
		{
			if (Targets[i].IsChased == false)
			{
				CombinationTargets.push_back(&Targets[i]);
				Target::ChasedAll = false;
			}
		}

		// Полный перебор вариантов полного пути преследователя и выбор оптимального
		do {
			// Расчеты для данной комбинации пути
			for (i = 0; i < CombinationTargets.size(); i++)
			{
				CombinationTargets[i]->CalculateAngle(chaser->Sx, chaser->Sy, chaser->V);
				if (i != 0)
				{																							// Возвращение цели на место
					CombinationTargets[i]->Sx = BufferXTarget; CombinationTargets[i]->Sy = BufferYTarget;	// Поскольку дальше в цикле цель i+1 смещается на время погони цели i
				}																							// и на следующей итерации цель, которая была i+1 становится i и возвращается на место благодаря буферу
				TotalTime = TotalTime + CombinationTargets[i]->ChaseTime;									// для вычисления общего времени пути
				chaser->Sx = CombinationTargets[i]->AimX; chaser->Sy = CombinationTargets[i]->AimY;			// чтобы дальше вычисления велись от координат пойманной цели
				if (i < CombinationTargets.size() - 1)
				{
					BufferXTarget = CombinationTargets[i + 1]->Sx; BufferYTarget = CombinationTargets[i + 1]->Sy;				// Запись в буфер координат цели i+1
					CombinationTargets[i + 1]->Sx = CombinationTargets[i + 1]->Sx + CombinationTargets[i + 1]->Vx * TotalTime;	// Сдвиг координаты x цели i + 1 по общему времени погони, для моделирования погони для данной комбинации пути 
					CombinationTargets[i + 1]->Sy = CombinationTargets[i + 1]->Sy + CombinationTargets[i + 1]->Vy * TotalTime;	// Сдвиг координаты н цели i + 1 по общему времени погони
				}
			}
			// Определение лучшей комбинации пути
			if (BestTotalTime > TotalTime)
			{
				BestTotalTime = TotalTime;
				BestCombinationTargets = CombinationTargets;
			}
			TotalTime = 0; // Сброс для вычисления времени для следующей комбинации
			chaser->Sx = BufferXChaser; chaser->Sy = BufferYChaser; // Возвращаем преследователя на его место

		} while (next_permutation(CombinationTargets.begin(), CombinationTargets.end()));	// Переставляет элементы массива местами (Пр: 123, 132, 213, 231...) 

		// Вывод упр. команд
		if (Target::ChasedAll)
		{
			cout << 0 << endl;
		}
		else
		{
			BestCombinationTargets[0]->CalculateAngle(chaser->Sx, chaser->Sy, chaser->V);
			cout << BestCombinationTargets[0]->ChaseAngle << endl;
		}

		cout << Target::ChasedAll << endl;
	}

	// Очистка памяти
	delete chaser;
	delete[] Targets;

	return 0;
}