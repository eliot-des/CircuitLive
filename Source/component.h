/*
  ==============================================================================

    component.h
    Created: 28 May 2024 10:07:09am
    Author:  eliot

  ==============================================================================
*/


#pragma once
#include "EigenWrapper.h"
#include "TaperFunctions.h"
//Forward declaration of System class to avoid circular dependencies
class System;

class Component {
public:
    unsigned start_node, end_node;

    Component(unsigned start_node, unsigned end_node);
    virtual ~Component() = default;

    virtual bool is_static()    const = 0;
    virtual bool is_variable()  const = 0;
    virtual bool is_dynamic()   const = 0;
    virtual bool is_nonlinear() const = 0;

    // Use to set all the non-zero entries of the sparse matrix A
    virtual void register_component(System& system) const = 0;
    // Use to create SparseMatrixEntry/VectorMatrixEntry objects to easily read/write a non-zero entry of the sparse matrix A or the vectors b and x
    virtual void setup(System& system) = 0;
    virtual void stamp_A(System& system) = 0;
    virtual void stamp_b(System& system) = 0;

    using Ptr = std::shared_ptr<Component>;
};

class Resistance : public Component {
public:
    double admittance;

    Resistance(unsigned start_node, unsigned end_node, double resistance);

    virtual bool is_static() const override { return true; }
    virtual bool is_variable() const override { return false; }
    virtual bool is_dynamic() const override { return false; }
    virtual bool is_nonlinear() const override { return false; }

    virtual void register_component(System& system) const override;
    virtual void setup(System& system) override;
    virtual void stamp_A(System& system) override;
    virtual void stamp_b(System& system) override {};

protected:
    SparseMatrixEntry A_start_start, A_start_end, A_end_start, A_end_end;
};

class Varistance : public Resistance {
public:
    double alpha;//knob position
    unsigned int knobIdx; //index of the RotarySlider that controls the resistance in Juce GUI
    std::function<double(double)> taperFunction; // function pointer for taper function

    Varistance(unsigned start_node, unsigned end_node, double resistance, unsigned int knobIdx, char taper='\0');

    virtual bool is_static() const override { return false; }
    virtual bool is_variable()  const override { return true; }

    virtual void stamp_A(System& system) override;

private:
    void setTaper(char taper);
};


class Potentiometer : public Component {
public:
    unsigned middle_node;
    double resistance;
    double alpha;
    unsigned int knobIdx;
    std::function<double(double)> taperFunction;

    Potentiometer(unsigned start_node, unsigned end_node, unsigned middle_node, double resistance, unsigned int knobIdx, char taper = '\0');

    virtual bool is_static() const override { return false; }
    virtual bool is_variable() const override { return true; }
    virtual bool is_dynamic() const override { return false; }
    virtual bool is_nonlinear() const override { return false; }

    virtual void register_component(System& system) const override;
    virtual void setup(System& system) override;
    virtual void stamp_A(System& system) override;
    virtual void stamp_b(System& system) override {};

private:
    SparseMatrixEntry A_start_start, A_end_end, A_middle_middle, A_start_middle, A_middle_start, A_end_middle, A_middle_end;
    void setTaper(char taper);
};


class ReactiveComponent : public Component {
public:
    unsigned index;
    double value;
    double resistance;
    double voltage;

    ReactiveComponent(unsigned start_node, unsigned end_node, double value, unsigned index);

    virtual bool is_static()    const override { return false; }
    virtual bool is_variable()  const override { return false; }
    virtual bool is_dynamic()   const override { return true; }
    virtual bool is_nonlinear() const override { return false; }

    virtual void register_component(System& system) const override;
    virtual void setup(System& system) override;
    virtual void setResistance(double Ts) = 0;
    virtual void updateVoltage() = 0;
    virtual void stamp_A(System& system) override;
    virtual void stamp_b(System& system) override;

protected:
    SparseMatrixEntry A_index_start, A_index_end, A_start_index, A_end_index, A_index_index;
    VectorEntry b_index;
    VectorEntry x_start, x_end, x_index;
};

class Capacitor : public ReactiveComponent {
public:
    Capacitor(unsigned start_node, unsigned end_node, double value, unsigned index);
    virtual void setResistance(double Ts) override;
    virtual void updateVoltage() override;
};

class Inductance : public ReactiveComponent {
public:
    Inductance(unsigned start_node, unsigned end_node, double value, unsigned index);
    virtual void setResistance(double Ts) override;
    virtual void updateVoltage() override;
};

class VoltageSource : public Component {
public:
    double voltage;
    unsigned index;

    VoltageSource(unsigned start_node, unsigned end_node, double voltage, unsigned index);

    virtual bool is_static()    const override { return true; }
    virtual bool is_variable()  const override { return false; }
    virtual bool is_dynamic()   const override { return false; }
    virtual bool is_nonlinear() const override { return false; }

    virtual void register_component(System& system) const override;
    virtual void setup(System& system) override;
    virtual void stamp_A(System& system) override;
    virtual void stamp_b(System& system) override;

private:
    SparseMatrixEntry A_index_start, A_index_end, A_start_index, A_end_index;
    VectorEntry b_index;
};

class ExternalVoltageSource : public VoltageSource {
public:
    virtual bool is_static() const override { return false; }
    virtual bool is_dynamic() const override { return true; }

    ExternalVoltageSource(unsigned start_node, unsigned end_node, double value, unsigned index);
    void updateVoltage(double new_voltage);
};

class VoltageProbe : public Component {
public:
    double voltage;

    VoltageProbe(unsigned start_node, unsigned end_node);

    virtual bool is_static()    const override { return false; }
    virtual bool is_variable()  const override { return false; }
    virtual bool is_dynamic()   const override { return false; }
    virtual bool is_nonlinear() const override { return false; }

    virtual void register_component(System& system) const override {}
    virtual void setup(System& system) override;
    virtual void stamp_A(System& system) override {}
    virtual void stamp_b(System& system) override {}

    double getVoltage();

private:
    VectorEntry x_start, x_end;
};


class IdealOPA : public Component {
public:
    unsigned output_node;
    unsigned index;

    IdealOPA(unsigned start_node, unsigned end_node, unsigned output_node, unsigned index);

    virtual bool is_static()    const override { return true; }
    virtual bool is_variable()  const override { return false; }
    virtual bool is_dynamic()   const override { return false; }
    virtual bool is_nonlinear() const override { return false; }

    virtual void register_component(System& system) const override;
    virtual void setup(System& system) override;
    virtual void stamp_A(System& system) override;
    virtual void stamp_b(System& system) override {};

private:
    SparseMatrixEntry A_out_index, A_index_start, A_index_end;

};


class Diode : public Component {
public:
    double Vt;      //thermal voltage
    double N;       //emission coefficient of the diode
    double Is;      //saturation current
    double N_Vt;    //N*Vt

    double Id;      //current through the diode
    double voltage; //voltage across the diode

    double Ieq;     //equivalent current
    double Geq;	    //equivalent conductance

    Diode(unsigned start_node, unsigned end_node);

    virtual bool is_static()    const override { return false; }
    virtual bool is_variable()  const override { return false; }
    virtual bool is_dynamic()   const override { return false; }
    virtual bool is_nonlinear() const override { return true; }

    virtual void register_component(System& system) const override;
    virtual void setup(System& system) override;
    virtual void stamp_A(System& system) override;
    virtual void stamp_b(System& system) override;


private:
    SparseMatrixEntry A_start_start, A_start_end, A_end_start, A_end_end;
    VectorEntry b_start, b_end;
    VectorEntry x_start, x_end;
};










class Transformer : public Component {
public:
    unsigned index;
    unsigned node_1, node_2, node_3, node_4;
    double ratio;

    Transformer(unsigned node_1, unsigned node_2, unsigned node_3, unsigned node_4, double ratio, unsigned index);

    virtual bool is_static()    const override { return true; }
    virtual bool is_variable()  const override { return false; }
    virtual bool is_dynamic()   const override { return false; }
    virtual bool is_nonlinear() const override { return false; }

    virtual void register_component(System& system) const override;
    virtual void setup(System& system) override;
    virtual void stamp_A(System& system) override;
    virtual void stamp_b(System& system) override;
private:
    SparseMatrixEntry A_index_1, A_index_2, A_index_3, A_index_4, A_1_index, A_2_index, A_3_index, A_4_index;
};

