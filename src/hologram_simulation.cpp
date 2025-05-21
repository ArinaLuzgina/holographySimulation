#include <iostream>
#include <cmath>
#include <complex>
#include <vector>
#include <fftw3.h>
#include <Eigen/Dense>
#include <fstream>

using namespace Eigen;
using namespace std;

typedef complex<double> Complex;

// Параметры симуляции
const int num_pixels = 1024;       // Разрешение голограммы
const double size_h = 0.1;           // Размер голограммы (метры)
const double wavelength = 532e-9;  // Длина волны (м)
const double k = 2 * M_PI / wavelength;

// Расчет опорной волны (плоская волна под углом)
MatrixXcd calculate_reference_wave(double reference_angle) {
    MatrixXcd reference_wave(num_pixels, num_pixels);
    double dx = size_h / num_pixels;
    
    for (int i = 0; i < num_pixels; ++i) {
        for (int j = 0; j < num_pixels; ++j) {
            double x = (i - num_pixels/2) * dx;
            double y = (j - num_pixels/2) * dx;
            double phase = k * (x * sin(reference_angle) + y * 0);  // Угол в плоскости XZ
            reference_wave(i, j) = polar(1.0, phase);
        }
    }
    return reference_wave;
}

// Волна от точечного источника
MatrixXcd calculate_object_wave(double x0, double y0, double z0) {
    MatrixXcd object_wave(num_pixels, num_pixels);
    double dx = size_h / num_pixels;
    
    for (int i = 0; i < num_pixels; ++i) {
        for (int j = 0; j < num_pixels; ++j) {
            double x = (i - num_pixels/2) * dx;
            double y = (j - num_pixels/2) * dx;
            double r = sqrt(pow(x - x0, 2) + pow(y - y0, 2) + z0*z0);
            object_wave(i, j) = polar(1.0 / r, k * r);  // Сферическая волна
        }
    }
    return object_wave;
}

// Расчет коэффициента пропускания голограммы
MatrixXd calculate_transmittance(const MatrixXcd& object_wave, const MatrixXcd& reference_wave) {
    MatrixXcd interference = object_wave + reference_wave;
    return interference.cwiseAbs2();  // |object + reference|^2
}

// Восстановление изображения через FFT
MatrixXd reconstruct_hologram(const MatrixXd& hologram, const MatrixXcd& reference_wave) {
    // Умножаем голограмму на сопряженную опорную волну
    MatrixXcd hologram_complex = hologram.cast<Complex>().cwiseProduct(reference_wave.conjugate());
    
    // FFT
    fftw_complex *in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * num_pixels * num_pixels);
    fftw_complex *out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * num_pixels * num_pixels);
    
    fftw_plan plan = fftw_plan_dft_2d(num_pixels, num_pixels, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
    
    // Заполнение входных данных
    for (int i = 0; i < num_pixels; ++i) {
        for (int j = 0; j < num_pixels; ++j) {
            in[i*num_pixels + j][0] = real(hologram_complex(i, j));
            in[i*num_pixels + j][1] = imag(hologram_complex(i, j));
        }
    }
    
    fftw_execute(plan);
    
    // Извлечение результата
    MatrixXd reconstructed_intensity(num_pixels, num_pixels);
    for (int i = 0; i < num_pixels; ++i) {
        for (int j = 0; j < num_pixels; ++j) {
            double re = out[i*num_pixels + j][0];
            double im = out[i*num_pixels + j][1];
            reconstructed_intensity(i, j) = re*re + im*im ;  // Логарифмический масштаб
        }
    }
    
    fftw_destroy_plan(plan);
    fftw_free(in);
    fftw_free(out);
    
    return reconstructed_intensity;
}

// Сохранение матрицы в файл (для визуализации в Python/Matplotlib)
void save_matrix(const MatrixXd& matrix, const string& filename) {
    ofstream file(filename);
    // Создаем формат для CSV: разделитель запятая, без выравнивания
    IOFormat CSVFormat(StreamPrecision, DontAlignCols, ", ", "\n");
    file << matrix.format(CSVFormat);
    file.close();
}

int main() {
    // Параметры
    double reference_angle = 0.1;  // Угол опорной волны (радианы)
    double x0 = 0.0, y0 = 0.0, z0 = 0.1;  // Положение точечного источника
    
    // Расчет волн
    MatrixXcd reference_wave = calculate_reference_wave(reference_angle);
    MatrixXcd object_wave = calculate_object_wave(x0, y0, z0);
    
    // Голограмма
    MatrixXd hologram = calculate_transmittance(object_wave, reference_wave);
    save_matrix(hologram, "hologram.csv");
    
    // Восстановление изображения
    MatrixXd reconstructed = reconstruct_hologram(hologram, reference_wave);
    save_matrix(reconstructed, "reconstructed.csv");
    
    return 0;
}