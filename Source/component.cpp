/*
  ==============================================================================

    component.cpp
    Created: 28 May 2024 10:07:09am
    Author:  eliot

  ==============================================================================
*/
#include "component.h"
#include "component.h"
#include "system.h"

Component::Component(unsigned start_node, unsigned end_node)
    : start_node(start_node), end_node(end_node) {}

//=================================================================================================
//=================================================================================================

Resistance::Resistance(unsigned start_node, unsigned end_node, double resistance)
    : Component(start_node, end_node), admittance(1.0 / resistance) {}

void Resistance::register_component(System& system) const {
    system.setSparseMatrixEntry(start_node, end_node);
    system.setSparseMatrixEntry(end_node, start_node);
    system.setSparseMatrixEntry(start_node, start_node);
    system.setSparseMatrixEntry(end_node, end_node);
}

void Resistance::setup(System& system) {
    A_start_start.init(system.A, start_node, start_node);
    A_start_end.init(system.A, start_node, end_node);
    A_end_start.init(system.A, end_node, start_node);
    A_end_end.init(system.A, end_node, end_node);
}

void Resistance::stamp_A(System& system) {
    A_start_start += admittance;
    A_end_end += admittance;
    A_start_end -= admittance;
    A_end_start -= admittance;
}

//=================================================================================================
//=================================================================================================

Varistance::Varistance(unsigned start_node, unsigned end_node, double resistance, unsigned int knobIdx, char taper)
    : Resistance(start_node, end_node, resistance), knobIdx(knobIdx), alpha(0) {
    setTaper(taper);
}

void Varistance::setTaper(char taper) {
    switch (taper) {
    case 'A':
        taperFunction = TaperFunctions::applyTaperA;
        break;
    case 'B':
        taperFunction = TaperFunctions::applyTaperB;
        break;
    default:
        taperFunction = [](double x) { return x; };
        break;
    }
}


void Varistance::stamp_A(System& system) {
    alpha = system.knobPositions[knobIdx];
    alpha = taperFunction(alpha);
    double newAdmittance = (admittance / alpha) + 0.0001;

    // Use the updated admittance directly instead of scaling the existing value
    A_start_start += newAdmittance;
    A_end_end += newAdmittance;
    A_start_end -= newAdmittance;
    A_end_start -= newAdmittance;
}

//=================================================================================================
//=================================================================================================
//the middle node correspond to the wiper node of the potentiometer
Potentiometer::Potentiometer(unsigned start_node, unsigned end_node, unsigned middle_node, double resistance, unsigned int knobIdx, char taper)
    : Component(start_node, end_node), middle_node(middle_node), resistance(resistance), knobIdx(knobIdx), alpha(0) {
    setTaper(taper);
}


void Potentiometer::setTaper(char taper) {
    switch (taper) {
    case 'A':
        taperFunction = TaperFunctions::applyTaperA;
        break;
    case 'B':
        taperFunction = TaperFunctions::applyTaperB;
        break;
    default:
        taperFunction = [](double x) { return x; };
        break;
    }
}

void Potentiometer::register_component(System& system) const {
    system.setSparseMatrixEntry(start_node, start_node);
    system.setSparseMatrixEntry(middle_node, middle_node);
    system.setSparseMatrixEntry(end_node, end_node);
    system.setSparseMatrixEntry(start_node, middle_node);
    system.setSparseMatrixEntry(middle_node, start_node);
    system.setSparseMatrixEntry(end_node, middle_node);
    system.setSparseMatrixEntry(middle_node, end_node);

}

void Potentiometer::setup(System& system) {
    A_start_start.init(system.A, start_node, start_node);
    A_end_end.init(system.A, end_node, end_node);
    A_middle_middle.init(system.A, middle_node, middle_node);
    A_start_middle.init(system.A, start_node, middle_node);
    A_middle_start.init(system.A, middle_node, start_node);
    A_end_middle.init(system.A, end_node, middle_node);
    A_middle_end.init(system.A, middle_node, end_node);
}

void Potentiometer::stamp_A(System& system) {
    alpha = system.knobPositions[knobIdx];
    alpha = taperFunction(alpha);

    double p_plus  = 1 / (resistance * alpha + 0.0001);
    double p_minus = 1 / (resistance * (1 - alpha) + 0.0001);

    A_start_start += p_plus;
    A_end_end += p_minus;
    A_middle_middle += p_plus + p_minus;
    A_start_middle -= p_plus;
    A_middle_start -= p_plus;
    A_end_middle -= p_minus;
    A_middle_end -= p_minus;
}


//=================================================================================================
//=================================================================================================

ReactiveComponent::ReactiveComponent(unsigned start_node, unsigned end_node, double value, unsigned index)
    : Component(start_node, end_node), value(value), index(index), resistance(0), voltage(0) {}

void ReactiveComponent::register_component(System& system) const {
    unsigned n = system.n;

    system.setSparseMatrixEntry(n + index, start_node);
    system.setSparseMatrixEntry(n + index, end_node);
    system.setSparseMatrixEntry(start_node, n + index);
    system.setSparseMatrixEntry(end_node, n + index);
    system.setSparseMatrixEntry(n + index, n + index);
}

void ReactiveComponent::setup(System& system) {
    unsigned n = system.n;

    A_index_start.init(system.A, n + index, start_node);
    A_index_end.init(system.A, n + index, end_node);
    A_start_index.init(system.A, start_node, n + index);
    A_end_index.init(system.A, end_node, n + index);
    A_index_index.init(system.A, n + index, n + index);

    b_index.init(system.b, n + index);

    x_start.init(system.x, start_node);
    x_end.init(system.x, end_node);
    x_index.init(system.x, n + index);
}

void ReactiveComponent::stamp_A(System& system) {
    A_index_start = 1;
    A_index_end = -1;
    A_start_index = 1;
    A_end_index = -1;
    A_index_index = -resistance;
}

void ReactiveComponent::stamp_b(System& system) {
    updateVoltage();
    b_index = voltage;
}

//=================================================================================================
//=================================================================================================

Capacitor::Capacitor(unsigned start_node, unsigned end_node, double value, unsigned index)
    : ReactiveComponent(start_node, end_node, value, index) {}

void Capacitor::setResistance(double Ts) {
    resistance = (Ts / (2 * value));
}

void Capacitor::updateVoltage() {
    voltage = (x_start - x_end) + resistance * x_index;
}

//=================================================================================================
//=================================================================================================

Inductance::Inductance(unsigned start_node, unsigned end_node, double value, unsigned index)
    : ReactiveComponent(start_node, end_node, value, index) {}

void Inductance::setResistance(double Ts) {
    resistance = (2 * value) / Ts;
}

void Inductance::updateVoltage() {
    voltage = -((x_start - x_end) + resistance * x_index);
}

//=================================================================================================
//=================================================================================================

VoltageSource::VoltageSource(unsigned start_node, unsigned end_node, double voltage, unsigned index)
    : Component(start_node, end_node), voltage(voltage), index(index) {}

void VoltageSource::register_component(System& system) const {
    unsigned n = system.n;

    system.setSparseMatrixEntry(n + index, start_node);
    system.setSparseMatrixEntry(n + index, end_node);
    system.setSparseMatrixEntry(start_node, n + index);
    system.setSparseMatrixEntry(end_node, n + index);
}

void VoltageSource::setup(System& system) {
    unsigned n = system.n;

    A_index_start.init(system.A, n + index, start_node);
    A_index_end.init(system.A, n + index, end_node);
    A_start_index.init(system.A, start_node, n + index);
    A_end_index.init(system.A, end_node, n + index);

    b_index.init(system.b, n + index);
}

void VoltageSource::stamp_A(System& system) {
    A_index_start = 1;
    A_index_end = -1;
    A_start_index = 1;
    A_end_index = -1;
}

void VoltageSource::stamp_b(System& system) {
    b_index = voltage;
}

//=================================================================================================
//=================================================================================================

ExternalVoltageSource::ExternalVoltageSource(unsigned start_node, unsigned end_node, double voltage, unsigned index)
    : VoltageSource(start_node, end_node, voltage, index) {}

void ExternalVoltageSource::updateVoltage(double new_voltage) {
    voltage = new_voltage;
}

//=================================================================================================
//=================================================================================================

VoltageProbe::VoltageProbe(unsigned start_node, unsigned end_node)
    : Component(start_node, end_node), voltage(0) {}

void VoltageProbe::setup(System& system) {
    x_start.init(system.x, start_node);
    x_end.init(system.x, end_node);
}

double VoltageProbe::getVoltage() {
    return  x_start - x_end;
}

//=================================================================================================
//=================================================================================================

IdealOPA::IdealOPA(unsigned start_node, unsigned end_node, unsigned output_node, unsigned index)
    : Component(start_node, end_node), output_node(output_node), index(index) {}

void IdealOPA::register_component(System& system) const {
    unsigned n = system.n;

    system.setSparseMatrixEntry(output_node, n + index);
    system.setSparseMatrixEntry(n + index, start_node);
    system.setSparseMatrixEntry(n + index, end_node);
}

void IdealOPA::setup(System& system) {
    unsigned n = system.n;

    A_out_index.init(system.A, output_node, n + index);
    A_index_start.init(system.A, n + index, start_node);
    A_index_end.init(system.A, n + index, end_node);
}

void IdealOPA::stamp_A(System& system) {
    A_out_index = 1;
    A_index_start = 1;
    A_index_end = -1;
}

//=================================================================================================
//=================================================================================================

Diode::Diode(unsigned start_node, unsigned end_node)
    : Component(start_node, end_node) {

    //Diode parameters based on the characteristics of the 1N34A model

    N = 1.6;	   //ideality factor
    Is = 2.6e-6;   //reverse saturation current

    Vt = 25.852e-3;  //thermal voltage at approx. 300 Kelvin
    N_Vt = N * Vt;  //N*Vt

    Id = 0;         //current through the diode
    voltage = 0;    //voltage across the diode

    Ieq = 0;        //equivalent current
    Geq = 0;	    //equivalent conductance
}

void Diode::register_component(System& system) const {
    system.setSparseMatrixEntry(start_node, start_node);
    system.setSparseMatrixEntry(end_node, end_node);
    system.setSparseMatrixEntry(start_node, end_node);
    system.setSparseMatrixEntry(end_node, start_node);
}

void Diode::setup(System& system) {

    A_start_start.init(system.A, start_node, start_node);
    A_start_end.init(system.A, start_node, end_node);
    A_end_start.init(system.A, end_node, start_node);
    A_end_end.init(system.A, end_node, end_node);


    x_start.init(system.x, start_node);
    x_end.init(system.x, end_node);

    b_start.init(system.b, start_node);
    b_end.init(system.b, end_node);
}

void Diode::stamp_A(System& system) {
    voltage = x_start - x_end;
    Id = Is * std::expm1(voltage / N_Vt);
    Geq = (Is / N_Vt) * std::exp(voltage / N_Vt);
    Ieq = Id - Geq * voltage;

    A_start_start += Geq;
    A_end_end += Geq;
    A_start_end -= Geq;
    A_end_start -= Geq;
}

void Diode::stamp_b(System& system) {
    b_start -= Ieq;
    b_end += Ieq;
}

//=================================================================================================
//=================================================================================================

Transformer::Transformer(unsigned node_1, unsigned node_2, unsigned node_3, unsigned node_4, double ratio, unsigned index)
	: Component(node_1, node_2), node_1(node_1), node_2(node_2), node_3(node_3), node_4(node_4), ratio(ratio), index(index) {}
//not optimal to have variables node_1 and node_2 since this is already defined in the base class, but this is easier to read and understand for a quadrupole

void Transformer::register_component(System& system) const {
    unsigned n = system.n;

    system.setSparseMatrixEntry(n + index, node_1);
    system.setSparseMatrixEntry(n + index, node_2);
    system.setSparseMatrixEntry(n + index, node_3);
    system.setSparseMatrixEntry(n + index, node_4);
    system.setSparseMatrixEntry(node_1, n + index);
    system.setSparseMatrixEntry(node_2, n + index);
    system.setSparseMatrixEntry(node_3, n + index);
    system.setSparseMatrixEntry(node_4, n + index);
}

void Transformer::setup(System& system) {
	unsigned n = system.n;

	A_index_1.init(system.A, n + index, node_1);
	A_index_2.init(system.A, n + index, node_2);
	A_index_3.init(system.A, n + index, node_3);
	A_index_4.init(system.A, n + index, node_4);
	A_1_index.init(system.A, node_1, n + index);
	A_2_index.init(system.A, node_2, n + index);
	A_3_index.init(system.A, node_3, n + index);
	A_4_index.init(system.A, node_4, n + index);
}

void Transformer::stamp_A(System& system) {
    A_1_index = 1;
    A_2_index = -1;
    A_3_index = ratio;
    A_4_index = -ratio;

	A_index_1 = 1;
	A_index_2 = -1;
	A_index_3 = -ratio;
	A_index_4 = ratio;
}