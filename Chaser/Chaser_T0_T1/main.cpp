#include <iostream>
#include <cmath>

using namespace std;

class Point // Базовый класс - точка, хранит координаты и вектора скорости объектов
{
public:
	double Sx, Sy, Vx, Vy;
public:
	virtual void UpdateParameters() = 0;
};

class Chaser : public Point // Дочерний класс - преследователь
{
public:
	double V; // скорость преследователя, получаемая от отладчика
public:
	void SetSpeed()
	{
		cin >> V;
	}
	void UpdateParameters()
	{
		cin >> Sx >> Sy >> Vx >> Vy;
	}
};

class Target : public Point			// дочерний класс - цель
{
public:
	bool IsChased;					// поймана ли цель
	double TimeChase, AngleChase;	// время преследования цели и угол преследования
	static int CountTargets;		// кол-во целей
	static int CountChased;			// кол-во пойманых целей
	static bool ChasedAll;			// пойманы ли все цели
public:
	void UpdateParameters()
	{
		cin >> Sx >> Sy >> Vx >> Vy >> IsChased;
		if (IsChased == true)
		{
			CountChased++;
		}
	}

	// вычисление параметров (TimeChase, AngleChase) для "охоты" на упреждение
	// т.е. преследователь не гонится за целью, а наперед просчитывает оптимальную точку столкновения
	// для примера, уравение кинематики для оси Х: SxП + VxП * t = SxЦ + VxЦ * t
	// но поскольку неизвестных два (скорость преследователя для каждоый оси и t), то преобразуем все в квадратное уравнение и находим t
	void CalculateChaseParameters(double SxChaser, double SyChaser, double VChaser)
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
				TimeChase = t1;
			}
			else
			{
				TimeChase = t2;
			}
		}
		else if (t1 > 0)
		{
			TimeChase = t1;
		}
		else if (t2 > 0)
		{
			TimeChase = t2;
		}
		else
		{
			TimeChase = sqrt(Dx * Dx + Dy * Dy) / VChaser;
		}

		double AimX = Sx + Vx * TimeChase;						// точка столкновения по оси Х
		double AimY = Sy + Vy * TimeChase;						// точка столкновения по оси Y
		AngleChase = atan2(AimY - SyChaser, AimX - SxChaser);	// вычисление угла для попадания в точку столкновения
	}
};
// инициализация статических переменных
int Target::CountTargets = 0;
int Target::CountChased = 0;
bool Target::ChasedAll = false;

int main()
{
	double t;	// время моделирования // пока непонятно как его использовать, если честно
	int i;

	//получение начальных данных от отладчика
	Chaser* chaser = new Chaser;	// преследователь
	chaser->SetSpeed();				// установка скорости преследователю

	cin >> Target::CountTargets;						// кол-во целей
	Target* targets = new Target[Target::CountTargets]; // массив целей
	Target* CurrentTarget;								// указатель на выбранную цель для преследования

	//начало моделирования
	while (Target::ChasedAll == false)
	{
		Target::CountChased = 0; // обнуление, поскольку при обновлении параметров целей счетчик заново заполняется
		CurrentTarget = nullptr; // обнуление

		// обновление параметров моделирования
		cin >> t;
		chaser->UpdateParameters();
		for (i = 0; i < Target::CountTargets; i++)
		{
			targets[i].UpdateParameters();
		}

		// выбор цели для преследования
		for (i = 0; i < Target::CountTargets; i++)
		{
			if (targets[i].IsChased == false) // условие для отсеивания уже пойманных целей
			{
				// получение времени для поимки цели
				targets[i].CalculateChaseParameters(chaser->Sx, chaser->Sy, chaser->V);

				// выбор цели для преследования по кратчайшему времени
				if (CurrentTarget == nullptr || CurrentTarget->TimeChase > targets[i].TimeChase)
				{
					CurrentTarget = &targets[i];
				}
			}
		}

		// получение угла для поимки выбранной цели и выдача его в отладчик
		CurrentTarget->CalculateChaseParameters(chaser->Sx, chaser->Sy, chaser->V);
		cout << CurrentTarget->AngleChase << endl;

		// условие завершения моделирования, если все пойманы выдача в отладчик 1 (завершение программы)
		if (Target::CountChased == Target::CountTargets)
		{
			Target::ChasedAll = true;
		}
		cout << Target::ChasedAll << endl;
	}

	// очистка памяти
	delete chaser;
	delete[] targets;

	return 0;
}