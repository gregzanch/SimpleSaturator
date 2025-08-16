#include "plugin.hpp"


struct Basic : Module {
	enum ParamId {
		THRESHOLD_PARAM,
		PARAMS_LEN
	};
	enum InputId {
		IN_INPUT,
		INPUTS_LEN
	};
	enum OutputId {
		OUT_OUTPUT,
		OUTPUTS_LEN
	};
	enum LightId {
		LIGHTS_LEN
	};

	Basic() {
		config(PARAMS_LEN, INPUTS_LEN, OUTPUTS_LEN, LIGHTS_LEN);
		configParam(THRESHOLD_PARAM, 0.f, 5.f, 0.f, "");
		configInput(IN_INPUT, "");
		configOutput(OUT_OUTPUT, "");
	}

	float clippedValue(float voltage, bool above) {
		float threshold = params[THRESHOLD_PARAM].getValue();
		float numerator = above ? 5 * (voltage - threshold) : 5 * (voltage + threshold);
		float kernel = numerator / (5 + 5 * powf(numerator / (5 - threshold), 2.f));
		return above ? threshold + kernel : kernel - threshold;
	}

	void process(const ProcessArgs& args) override {
				// Compute the frequency from the pitch parameter and input
		float threshold = params[THRESHOLD_PARAM].getValue();
		float inputVoltage = inputs[IN_INPUT].getVoltage();
		if(inputVoltage >= -threshold && inputVoltage <= threshold) {
			outputs[OUT_OUTPUT].setVoltage(inputVoltage);
		} else if(inputVoltage > threshold) {
			outputs[OUT_OUTPUT].setVoltage(clippedValue(inputVoltage, true));
		} else {
			outputs[OUT_OUTPUT].setVoltage(clippedValue(inputVoltage, false));
		}
	}
};


struct BasicWidget : ModuleWidget {
	BasicWidget(Basic* module) {
		setModule(module);
		setPanel(createPanel(asset::plugin(pluginInstance, "res/Basic.svg")));

		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, 0)));
		addChild(createWidget<ScrewSilver>(Vec(RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));
		addChild(createWidget<ScrewSilver>(Vec(box.size.x - 2 * RACK_GRID_WIDTH, RACK_GRID_HEIGHT - RACK_GRID_WIDTH)));

		addParam(createParamCentered<RoundBlackKnob>(mm2px(Vec(20.373, 42.863)), module, Basic::THRESHOLD_PARAM));

		addInput(createInputCentered<PJ301MPort>(mm2px(Vec(20.373, 83.873)), module, Basic::IN_INPUT));

		addOutput(createOutputCentered<PJ301MPort>(mm2px(Vec(20.373, 107.685)), module, Basic::OUT_OUTPUT));
	}
};


Model* modelBasic = createModel<Basic, BasicWidget>("Basic");