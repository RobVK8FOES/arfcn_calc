#include <iostream>
#include <iomanip>
#include <vector>
#include <string>
#include <cstdlib>
#include <cmath>
#include <stdexcept>
#include <algorithm>
#include <cctype>

// Structure to hold the matched band and calculated frequency
struct FrequencyResult {
    std::string band;
    double frequencyMHz;
};

// Structure to hold the matched band and calculated ARFCN
struct ArfcnResult {
    std::string band;
    int arfcn;
};

// Function to calculate all possible downlink frequencies for a given ARFCN
std::vector<FrequencyResult> calculateDownlinkFrequencies(int arfcn) {
    std::vector<FrequencyResult> results;

    // Primary GSM 900 (P-GSM)
    if (arfcn >= 1 && arfcn <= 124) {
        results.push_back({"P-GSM 900", 935.0 + 0.2 * arfcn});
    }
    
    // GSM 850
    if (arfcn >= 128 && arfcn <= 251) {
        results.push_back({"GSM 850", 869.2 + 0.2 * (arfcn - 128)});
    }
    
    // GSM 450
    if (arfcn >= 259 && arfcn <= 293) {
        results.push_back({"GSM 450", 460.4 + 0.2 * (arfcn - 259)});
    }
    
    // GSM 480
    if (arfcn >= 306 && arfcn <= 340) {
        results.push_back({"GSM 480", 488.8 + 0.2 * (arfcn - 306)});
    }
    
    // DCS 1800
    if (arfcn >= 512 && arfcn <= 885) {
        results.push_back({"DCS 1800", 1805.2 + 0.2 * (arfcn - 512)});
    }
    
    // PCS 1900
    if (arfcn >= 512 && arfcn <= 810) {
        results.push_back({"PCS 1900", 1930.2 + 0.2 * (arfcn - 512)});
    }
    
    // Extended GSM 900 (E-GSM) and Railways GSM 900 (R-GSM)
    if (arfcn >= 955 && arfcn <= 1023) {
        results.push_back({"E-GSM / R-GSM 900", 935.0 + 0.2 * (arfcn - 1024)});
    }

    return results;
}

// Function to calculate matching ARFCNs for a given downlink frequency
std::vector<ArfcnResult> calculateARFCN(double frequencyMHz) {
    std::vector<ArfcnResult> results;
    double epsilon = 0.05; // 50 kHz tolerance for floating point inaccuracies

    // Helper lambda to reverse the calculation, verify limits, and check precision
    auto checkAndAdd = [&](std::string band, int min_arfcn, int max_arfcn, double calculated_arfcn, double base_freq, int offset) {
        int n = static_cast<int>(std::round(calculated_arfcn));
        if (n >= min_arfcn && n <= max_arfcn) {
            double expected_f = base_freq + 0.2 * (n - offset);
            if (std::abs(frequencyMHz - expected_f) <= epsilon) {
                results.push_back({band, n});
            }
        }
    };

    // P-GSM 900
    checkAndAdd("P-GSM 900", 1, 124, (frequencyMHz - 935.0) / 0.2, 935.0, 0);
    // GSM 850
    checkAndAdd("GSM 850", 128, 251, (frequencyMHz - 869.2) / 0.2 + 128, 869.2, 128);
    // GSM 450
    checkAndAdd("GSM 450", 259, 293, (frequencyMHz - 460.4) / 0.2 + 259, 460.4, 259);
    // GSM 480
    checkAndAdd("GSM 480", 306, 340, (frequencyMHz - 488.8) / 0.2 + 306, 488.8, 306);
    // DCS 1800
    checkAndAdd("DCS 1800", 512, 885, (frequencyMHz - 1805.2) / 0.2 + 512, 1805.2, 512);
    // PCS 1900
    checkAndAdd("PCS 1900", 512, 810, (frequencyMHz - 1930.2) / 0.2 + 512, 1930.2, 512);
    // E-GSM / R-GSM 900
    checkAndAdd("E-GSM / R-GSM 900", 955, 1023, (frequencyMHz - 935.0) / 0.2 + 1024, 935.0, 1024);

    return results;
}

void processARFCNInput(int arfcn) {
    if (arfcn < 0 || arfcn > 1023) {
        std::cerr << "Error: ARFCN " << arfcn << " is out of the valid global GSM range (0-1023)." << std::endl;
        return;
    }

    std::vector<FrequencyResult> frequencies = calculateDownlinkFrequencies(arfcn);

    if (frequencies.empty()) {
        std::cout << "ARFCN " << arfcn << " is unassigned or invalid in standard 2G GSM bands." << std::endl;
    } else {
        std::cout << "\nResults for ARFCN " << arfcn << ":" << std::endl;
        std::cout << "--------------------------------" << std::endl;
        for (size_t i = 0; i < frequencies.size(); ++i) {
            std::cout << "Band: " << std::left << std::setw(18) << frequencies[i].band 
                      << "Downlink: " << std::fixed << std::setprecision(1) 
                      << frequencies[i].frequencyMHz << " MHz" << std::endl;
        }
        std::cout << "--------------------------------\n" << std::endl;
    }
}

void processFrequencyInput(double frequencyMHz) {
    std::vector<ArfcnResult> arfcns = calculateARFCN(frequencyMHz);

    if (arfcns.empty()) {
        std::cout << "Frequency " << std::fixed << std::setprecision(1) << frequencyMHz 
                  << " MHz does not align with a valid 2G GSM ARFCN downlink." << std::endl;
    } else {
        std::cout << "\nResults for Downlink Frequency " << std::fixed << std::setprecision(1) << frequencyMHz << " MHz:" << std::endl;
        std::cout << "--------------------------------" << std::endl;
        for (size_t i = 0; i < arfcns.size(); ++i) {
            std::cout << "Band: " << std::left << std::setw(18) << arfcns[i].band 
                      << "ARFCN: " << arfcns[i].arfcn << std::endl;
        }
        std::cout << "--------------------------------\n" << std::endl;
    }
}

void evaluateAndRouteInput(const std::string& rawInput) {
    std::string cleanStr = rawInput;

    // 1. Remove all spaces so "1822.8 MHz" becomes "1822.8MHz"
    cleanStr.erase(std::remove_if(cleanStr.begin(), cleanStr.end(), 
        [](unsigned char c) { return std::isspace(c); }), cleanStr.end());

    if (cleanStr.empty()) {
        return;
    }

    // 2. Convert entirely to lowercase to easily catch "mhz", "MHZ", "Mhz", etc.
    std::transform(cleanStr.begin(), cleanStr.end(), cleanStr.begin(),
        [](unsigned char c) { return std::tolower(c); });

    bool explicitFrequency = false;

    // 3. Check if it ends with "mhz" and strip it off if it does
    if (cleanStr.length() >= 3 && cleanStr.substr(cleanStr.length() - 3) == "mhz") {
        explicitFrequency = true;
        cleanStr = cleanStr.substr(0, cleanStr.length() - 3);
    }

    try {
        double val = std::stod(cleanStr);
        
        // It's a frequency if: 
        // A) They explicitly typed 'mhz'
        // B) It contains a decimal point
        // C) It exceeds the max ARFCN of 1023
        if (explicitFrequency || cleanStr.find('.') != std::string::npos || val > 1023.0) {
            processFrequencyInput(val);
        } else {
            // Otherwise, it's a valid integer within the 0-1023 ARFCN range.
            processARFCNInput(static_cast<int>(val));
        }
    } catch (const std::invalid_argument&) {
        std::cerr << "Error: Invalid input. Please enter numbers (optionally followed by MHz)." << std::endl;
    } catch (const std::out_of_range&) {
        std::cerr << "Error: Number out of bounds." << std::endl;
    }
}

int main(int argc, char* argv[]) {
    // Process command-line arguments (supports inputs with or without spaces, e.g., "1822.8 MHz")
    if (argc > 1) {
        std::string combinedInput = "";
        for (int i = 1; i < argc; ++i) {
            combinedInput += argv[i];
            if (i < argc - 1) combinedInput += " ";
        }
        evaluateAndRouteInput(combinedInput);
        return 0;
    } 
    
    // Interactive mode if no arguments are provided
    std::string inputStr;
    std::cout << "Enter a 2G GSM ARFCN or Downlink Frequency (e.g., 600 or 1822.8 MHz): ";
    if (std::getline(std::cin, inputStr)) {
        evaluateAndRouteInput(inputStr);
    }

    return 0;
}
