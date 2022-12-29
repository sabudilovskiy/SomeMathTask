#include <iostream>
#include <CvPlot/cvplot.h>
#include <functional>
#include <utility>

double HalfPeriod(double frequency){
    return 1 / (2 * frequency);
}

size_t NumberHalfperiod(double time, double frequency){
    auto half_period = HalfPeriod(frequency);
    return time / half_period;
}

constexpr double accuracy = 0.000001;

std::vector<double> get_pair(double value){
    if (value < 0.5) return {0,1};
    else return {1, 0};
}

//std::vector<double> digital(double time, double frequency){
//    auto value = std::sin(2 * CV_PI * frequency * time);
//    if (1 - abs(value)  <=  accuracy || abs(value) <= accuracy){
//        return get_pair(value);
//    }
//    else{
//        auto cur_num = NumberHalfperiod(time, frequency);
//        if (cur_num % 2 == 0) return {1};
//        else return {0};
//    }
//}

double digital(double time, double frequency, size_t count_harmonicals){
    double sum = 0;
    auto w1 = 2 * CV_PI * frequency;
    for (size_t i = 0; i < count_harmonicals; i++){
        auto real_index = 2 * i + 1;
        sum += std::sin(w1 * real_index * time) / real_index ;
    }
    return 4 / CV_PI * sum;
}

double harmonical(double time, double frequency) {
    return std::sin(2 * CV_PI * frequency * time);
}

std::vector<double> range(double begin, double end, double step){
    if (begin > end) std::swap(begin, end);
    step = abs(step);
    size_t len = (end - begin) / step + 1;
    std::vector<double> result;
    result.reserve(len);
    while (begin < end){
        result.emplace_back(begin);
        begin+=step;
    }
    return result;
}

std::vector<double> my_transform(const std::vector<double>& from, std::function<double(double)> transformer){
    std::vector<double> result;
    result.reserve(from.size());
    std::transform(from.begin(),  from.end(), std::back_inserter(result), std::move(transformer));
    return result;
}

std::pair<std::vector<double>, std::vector<double>> my_monada(const std::vector<double>& from, std::function<std::vector<double>(double)> transformer){
    std::vector<double> result_x;
    std::vector<double> result_y;
    for (auto elem : from){
        auto tranformer_result = transformer(elem);
        for (auto tranformer_elem: tranformer_result){
            result_x.emplace_back(elem);
            result_y.emplace_back(tranformer_elem);
        }
    }
    return std::make_pair(std::move(result_x), std::move(result_y));
}

void CreateWindow(double frequency, std::vector<double>& x, std::vector<double>& y, std::string name){
    std::string title = std::string("Frequency: ").append(std::to_string(frequency));
    auto mat_digital = CvPlot::plot(x, y).title(title).render(400, 600);
    cv::imshow(name, mat_digital);
}

std::pair<std::vector<double>, std::vector<double>> range_harmonic(size_t count_harmonic, double frequency, double max_frequency){
    std::vector<double> result_x, result_y;
    result_x.reserve(count_harmonic);
    result_y.reserve(count_harmonic);
    const double a1 = 4 / CV_PI;
    for (int i = 0; i < count_harmonic; i++){
        auto cur_index = 2*i + 1;
        result_x.emplace_back(cur_index * frequency);
        result_y.emplace_back(0);
        result_x.emplace_back(cur_index * frequency);
        result_y.emplace_back(a1 / cur_index);
        result_x.emplace_back(cur_index * frequency);
        result_y.emplace_back(0);
    }
    result_x.emplace_back(max_frequency * count_harmonic * 2);
    result_y.emplace_back(0);
    return std::make_pair(std::move(result_x), std::move(result_y));
}

std::pair<std::vector<double>, std::vector<double>> GetHarmonicSpectre(double frequency, size_t count_harmonic){
    std::vector<double> x_spectre_harmonic = {0, frequency, frequency, frequency, static_cast<double>(count_harmonic * 2 + 1)};
    std::vector<double> y_spectre_harmonic = {0, 0, 1, 0, 0};
    return std::make_pair(std::move(x_spectre_harmonic), y_spectre_harmonic);
}

int main() {
    try{
        size_t count_harmonic = 20;
        std::vector<double> frequencies = {1.0, 2.0, 4.0, 8.0};
        auto max_frequency = *std::max_element(frequencies.begin(),  frequencies.end());
        while (true){
            for (auto frequency: frequencies){
                std::vector<double> x  = range(0, 1, 0.000001);
                std::vector<double> y_harmonic = my_transform(x,
                                                              [=](double time){return harmonical(time, frequency);});
                auto y_digital = my_transform(x,
                                              [=](double time){ return digital(time, frequency, count_harmonic);});
                auto [x_spectre_digital, y_spectre_digital] = range_harmonic(count_harmonic, frequency, max_frequency);
                CreateWindow(frequency, x, y_harmonic, "current harmonic, count: " + std::to_string(count_harmonic));
                CreateWindow(frequency, x, y_digital, "current digital");
                auto [x_spectre_harmonic, y_spectre_harmonic] = GetHarmonicSpectre(frequency, count_harmonic);
                CreateWindow(frequency, x_spectre_harmonic, y_spectre_harmonic, "current harmonic spectre, count: " + std::to_string(count_harmonic));

                CreateWindow(frequency, x_spectre_digital, y_spectre_digital, "current digital spectre");
                cv::waitKey();
            }
        }
    }
    catch (...){}
}
