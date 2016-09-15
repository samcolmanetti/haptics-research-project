#pragma once
class FitnessValue {
public:
	double fitness;
	int indexOfSample;

	FitnessValue(double fitnessValue, int index) : fitness(fitnessValue), indexOfSample(index) {}

	bool operator()(FitnessValue fv1, FitnessValue fv2) {
		return fv1.fitness < fv2.fitness;
	}
};