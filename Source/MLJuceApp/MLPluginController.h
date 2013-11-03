
// MadronaLib: a C++ framework for DSP applications.
// Copyright (c) 2013 Madrona Labs LLC. http://www.madronalabs.com
// Distributed under the MIT license: http://madrona-labs.mit-license.org/

#ifndef __ML_PLUGINCTRLR_H__
#define __ML_PLUGINCTRLR_H__

#include "MLProjectInfo.h"
#include "MLResponder.h"
#include "MLReporter.h"
#include "MLSignalReporter.h"
#include "MLAppView.h"
#include "MLPluginEditor.h"

class MLPluginController : 
	public MLResponder,
	public MLReporter,
	public MLSignalReporter,
    public MLPluginProcessor::Listener
{
public:
	MLPluginController(MLPluginProcessor* const pProcessor);
    ~MLPluginController();

	MLAppView* getView(); 
	void setView(MLAppView* v); 
	
	// things to do after View is set
	virtual void initialize();

	// from MLResponder
    virtual void buttonClicked (MLButton*);
 	void showMenu (MLSymbol menuName, MLSymbol instigatorName);
	virtual void menuItemChosen(MLSymbol menuName, int result);

	void dialValueChanged (MLDial*);
	void dialDragStarted (MLDial*);
	void dialDragEnded (MLDial*);
	void multiButtonValueChanged (MLMultiButton* pSlider, int idx);
	void multiSliderDragStarted (MLMultiSlider* pSlider, int idx);
	void multiSliderDragEnded (MLMultiSlider* pSlider, int idx);
	void multiSliderValueChanged (MLMultiSlider* pSlider, int idx);
    
    // MLPluginProcessor::Listener
    void scaleFilesChanged(const MLFileCollectionPtr fileCollection);

	void loadPresetByIndex (int idx);
	int getIndexOfPreset(const std::string* dir, const std::string* name);
	
	void prevPreset();
	void nextPreset();
	String getPresetString(int n);

	MLPluginProcessor* getProcessor() const { return mpProcessor; }
	
	MLMenu* createMenu(MLSymbol menuName);
	MLMenu* findMenuByName(MLSymbol menuName);	

	void doScaleMenu(int result);
	void doPresetMenu(int result);

#if ML_MAC
	void getPresetsToConvert(Array<File>* pResults);
	void convertPresets();
#endif // ML_MAC

protected:
	MLAppView* mpView;
    
 	juce::CriticalSection mViewLock;
	
	WeakReference<MLPluginController>::Master masterReference;
	friend class WeakReference<MLPluginController>;	

	void setupFormat();    
	void findFilesOneLevelDeep(File& startDir, String extension, Array<File>& results, MLMenu* pMenu);

private:
	MLPluginProcessor* mpProcessor; // contains Model
	
	String mCurrentPresetName;
	File mCurrentPresetFolder;

	std::string mVersionString;


	// stored indices for MIDI program changes-- hackish
	std::vector<File> mMIDIProgramFiles;

	MLMenuMapT mMenuMap; 	
	Array<File> mMenuPresetFiles;	
	int mPresetMenuStartItems;
	int mCurrentPresetIndex;

	void populatePresetMenu(); 
	void populateScaleMenu(const MLFileCollectionPtr fileCollection);	
 
};




#endif // __ML_PLUGINCTRLR_H__