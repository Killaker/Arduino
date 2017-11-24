/*
 * Request.h
 *
 *  Created on: May 14, 2016
 *      Author: Pat Deegan
 *      Part of the SerialUI Project
 *      Copyright (C) 2016 Pat Deegan, http://psychogenic.com
 * 
 */

#ifndef SERIALUI_SRC_INCLUDES_MENUITEM_REQUEST_H_
#define SERIALUI_SRC_INCLUDES_MENUITEM_REQUEST_H_

#include "../MenuItem.h"
#include "../SUITypes.h"
namespace SUI {
namespace MenuItem {
namespace Request {


template<class REQTYPE>
class RequestBase : public MenuItem::Base {
public:

	typedef  bool(*validatorCallback)(REQTYPE & newVal);
	virtual ~RequestBase() {}

	RequestBase(REQTYPE * val, SOVA_FLASHSTRING_PTR key_pstr, SOVA_FLASHSTRING_PTR help_pstr,
			validatorCallback vcb, MenuRequest_Callback cb=NULL) :
		MenuItem::Base(key_pstr, help_pstr, MenuItem::Type::Request), the_value(val), validator(vcb), value_changed_cb(cb) {}

/*
	RequestBase(REQTYPE * val, SOVA_FLASHSTRING_PTR key_pstr, SOVA_FLASHSTRING_PTR help_pstr, MenuRequest_Callback cb=NULL) :
		MenuItem::Base(key_pstr, help_pstr, MenuItem::Type::RequestBase), the_value(val), validator(NULL), value_changed_cb(cb) {}
*/
	REQTYPE * the_value;
	validatorCallback validator;







	MenuRequest_Callback value_changed_cb;

	virtual void showPrompt(Menu * callingMenu) = 0;
	virtual bool getValue(Menu * callingMenu, REQTYPE * v) = 0;

	void printValue(Menu * callingMenu, REQTYPE v)
	{
		if (callingMenu->driver()->echoCommands())
			callingMenu->driver()->println(v);

	}
	virtual void setValue(REQTYPE v)
	{
		*the_value = v;
	}

	virtual bool valueIsValid(REQTYPE & v)
	{
		SUI_UNUSED_PARAM(v);
		return true;
	}
	virtual void printMetaInfo(SUI::Menu * callingMenu)
	{
		SerialUI * driver = callingMenu->driver();
		driver->print(this->getString(StaticString::MetaInfoSeparator));

		driver->print(*the_value);
	}
	virtual Menu* call(Menu * callingMenu) {

		if (help) {
			callingMenu->driver()->println(help);
		}

		this->showPrompt(callingMenu);

		SUI_OUTPUTLN_DEBUG_DEFAULTSTREAM(F("Value req"));

		REQTYPE v;
		if (!this->getValue(callingMenu, &v) )
		{
			printValue(callingMenu, v);
			callingMenu->returnOK();
			return NULL;
		}


		bool determinedValid = true;
		printValue(callingMenu, v);
		if (validator)
		{
			SUI_OUTPUTLN_DEBUG_DEFAULTSTREAM(F("validating()"));
			if (! validator(v))
			{
				SUI_OUTPUTLN_DEBUG_DEFAULTSTREAM(F("Validator veto"));

				determinedValid = false;
			}
		} else if (! this->valueIsValid(v)) {
			SUI_OUTPUTLN_DEBUG_DEFAULTSTREAM(F("Internal veto"));
			determinedValid = false;

		}
		if (! determinedValid) {

			callingMenu->returnError(F("Invalid value"));
			return NULL;
		}
		this->setValue(v);

		if (value_changed_cb) {

			SUI_OUTPUTLN_DEBUG_DEFAULTSTREAM(F("val changed cb()"));
			value_changed_cb();
		}

		callingMenu->returnOK();
		return NULL;
	}

	virtual void printPrefix(Menu * callingMenu, SUI::Mode::Selection curMode)
	{
		if (curMode == SUI::Mode::User)
		{

			callingMenu->driver()->print(this->getString(StaticString::RequestPrefix));
		}

	}


};

class Char : public RequestBase<char> {
public:


	Char(char * val, SOVA_FLASHSTRING_PTR key_pstr, SOVA_FLASHSTRING_PTR help_pstr, bool(*vcb)(char & newVal),
			MenuRequest_Callback cb=NULL) :
		RequestBase<char>(val, key_pstr, help_pstr, vcb, cb) {}
/*
	Char(char * val, SOVA_FLASHSTRING_PTR key_pstr, SOVA_FLASHSTRING_PTR help_pstr, MenuRequest_Callback cb=NULL) :
		Request<char>(val, key_pstr, help_pstr, cb) {}
		*/
	virtual void showPrompt(Menu * callingMenu);
	virtual bool getValue(Menu * callingMenu, char * into);
	virtual void printPrefix(Menu * callingMenu, SUI::Mode::Selection curMode);
};

template<class REQTYPE>
class Numeric : public RequestBase<REQTYPE> {
public:

	Numeric(REQTYPE * val, SOVA_FLASHSTRING_PTR key_pstr, SOVA_FLASHSTRING_PTR help_pstr,
			bool(*vcb)(REQTYPE & newVal), MenuRequest_Callback cb=NULL) :
		RequestBase<REQTYPE>(val, key_pstr, help_pstr, vcb, cb) {}


	/*
	Numeric(REQTYPE * val, SOVA_FLASHSTRING_PTR key_pstr, SOVA_FLASHSTRING_PTR help_pstr, MenuRequest_Callback cb=NULL) :
		Request<REQTYPE>(val, key_pstr, help_pstr, cb) {}
		*/


	virtual ~Numeric() {}
	virtual void showPrompt(Menu * callingMenu) {

		callingMenu->driver()->showEnterNumericDataPrompt();
	}

	virtual char numericRequestSubtype() = 0;

	virtual void printPrefix(Menu * callingMenu, SUI::Mode::Selection curMode)
	{
		if (curMode == SUI::Mode::User)
		{
			this->RequestBase<REQTYPE>::printPrefix(callingMenu, curMode);
			return;
		}


		callingMenu->driver()->print(this->getString(StaticString::RequestNumPrefix));
		callingMenu->driver()->print(this->numericRequestSubtype());
	}
};


class Bool: public Numeric<bool>
{
public:

	Bool(bool * val, SOVA_FLASHSTRING_PTR key_pstr,
			SOVA_FLASHSTRING_PTR help_pstr,
			bool(*vcb)(bool & newVal),
			MenuRequest_Callback cb=NULL) :
		Numeric<bool>(val, key_pstr, help_pstr, vcb, cb) {}


	/*
	Bool(bool * val, SOVA_FLASHSTRING_PTR key_pstr, SOVA_FLASHSTRING_PTR help_pstr, MenuRequest_Callback cb=NULL) :
		Numeric<bool>(val, key_pstr, help_pstr, cb) {}
		*/


	virtual char numericRequestSubtype() ;
	virtual bool getValue(Menu * callingMenu, bool * into);
};

class Toggle: public Bool {


public:
	Toggle(bool * val, SOVA_FLASHSTRING_PTR key_pstr, SOVA_FLASHSTRING_PTR help_pstr, bool(*vcb)(bool & newVal),
			MenuRequest_Callback cb=NULL) :
				Bool(val, key_pstr, help_pstr, vcb, cb) {}


	virtual char numericRequestSubtype() ;

	virtual void showPrompt(Menu * callingMenu) ;

	virtual bool getValue(Menu * callingMenu, bool * into);

};

class Long: public Numeric<long int> {
public:


	Long(long int * val, SOVA_FLASHSTRING_PTR key_pstr, SOVA_FLASHSTRING_PTR help_pstr, bool(*vcb)(long int &),
			MenuRequest_Callback cb=NULL) :
		Numeric<long int>(val, key_pstr, help_pstr, vcb, cb) {}


	/*
	Long(long int * val, SOVA_FLASHSTRING_PTR key_pstr, SOVA_FLASHSTRING_PTR help_pstr, MenuRequest_Callback cb=NULL) :
		Numeric<long int>(val, key_pstr, help_pstr, cb) {}
		*/


	virtual char numericRequestSubtype() ;

	virtual bool getValue(Menu * callingMenu, long int * into);
};


class BoundedLong: public Long {
public:


	BoundedLong(long int * val, long int min, long int max, SOVA_FLASHSTRING_PTR key_pstr, SOVA_FLASHSTRING_PTR help_pstr,
			MenuRequest_Callback cb=NULL);

	BoundedLong(long int * val, long int min, long int max,
			SOVA_FLASHSTRING_PTR key_pstr, SOVA_FLASHSTRING_PTR help_pstr, BoundedLong::validatorCallback vcb,
			MenuRequest_Callback cb=NULL);

	void setStep(uint8_t val);

	virtual void printMetaInfo(SUI::Menu * callingMenu);


	virtual bool valueIsValid(long int & val) ;

	inline long int minimum() { return min_allowed;}
	inline long int maximum() { return max_allowed;}
	inline uint8_t step() { return step_val;}

protected:
	inline void setMinimum(long int i) { min_allowed = i;}
	inline void setMaximum(long int i) { max_allowed = i;}
	void ensureSaneBounds();

private:

	long int min_allowed;
	long int max_allowed;
	uint8_t step_val;



};

#define SUI_OPTIONSLIST_MAX_NUM		6

class OptionsList: public BoundedLong {
public:

	OptionsList(long int * val,
							SOVA_FLASHSTRING_PTR key_pstr, SOVA_FLASHSTRING_PTR help_pstr,
							SOVA_FLASHSTRING_PTR opt1, SOVA_FLASHSTRING_PTR opt2,
							SOVA_FLASHSTRING_PTR opt3=NULL, SOVA_FLASHSTRING_PTR opt4=NULL,
							SOVA_FLASHSTRING_PTR opt5=NULL, SOVA_FLASHSTRING_PTR opt6=NULL,
							MenuRequest_Callback cb=NULL);


	OptionsList(long int * val,
							SOVA_FLASHSTRING_PTR key_pstr, SOVA_FLASHSTRING_PTR help_pstr,
							OptionsList::validatorCallback vcb,
							SOVA_FLASHSTRING_PTR opt1, SOVA_FLASHSTRING_PTR opt2,
							SOVA_FLASHSTRING_PTR opt3=NULL, SOVA_FLASHSTRING_PTR opt4=NULL,
							SOVA_FLASHSTRING_PTR opt5=NULL, SOVA_FLASHSTRING_PTR opt6=NULL,
							MenuRequest_Callback cb=NULL);



	virtual void printMetaInfo(SUI::Menu * callingMenu);
	virtual char numericRequestSubtype() ;


	virtual void showPrompt(Menu * callingMenu) ;


	inline SOVA_FLASHSTRING_PTR optionBySelection(uint8_t idx) {
		if (idx < 1){
			return NULL;
		}
		return opt_list[idx - 1];
	}

	inline SOVA_FLASHSTRING_PTR optionByIndex(uint8_t idx) {
		if (idx >= maximum()){
			return NULL;
		}
		return opt_list[idx];
	}
private:
	void calculateBounds(long int* val);

	SOVA_FLASHSTRING_PTR  opt_list[SUI_OPTIONSLIST_MAX_NUM];



};






class ULong: public Numeric<long unsigned int> {
public:


	ULong(long unsigned int * val, SOVA_FLASHSTRING_PTR key_pstr, SOVA_FLASHSTRING_PTR help_pstr,bool(*vcb)(long unsigned int &), MenuRequest_Callback cb=NULL) :
		Numeric<long unsigned int>(val, key_pstr, help_pstr, vcb, cb) {}

	virtual char numericRequestSubtype() ;
	virtual bool getValue(Menu * callingMenu, long unsigned int * into) ;
};

class Float: public Numeric<float> {
public:


	Float(float * val, SOVA_FLASHSTRING_PTR key_pstr, SOVA_FLASHSTRING_PTR help_pstr,bool(*vcb)(float &), MenuRequest_Callback cb=NULL) :
		Numeric<float>(val, key_pstr, help_pstr, vcb, cb) {}

/*
	Float(float * val, SOVA_FLASHSTRING_PTR key_pstr, SOVA_FLASHSTRING_PTR help_pstr, MenuRequest_Callback cb=NULL) :
		Numeric<float>(val, key_pstr, help_pstr, cb) {}
*/
	virtual char numericRequestSubtype() ;

	virtual bool getValue(Menu * callingMenu, float * into) ;
};


// some, but not all, compilers hate having
// templates as <::whatever> so we use a typedef
typedef ::String TopLevString;
class String: public RequestBase<TopLevString> {
public:


	String(::String* val, uint8_t maxLength, SOVA_FLASHSTRING_PTR key_pstr, SOVA_FLASHSTRING_PTR help_pstr, bool(*vcb)(::String &), MenuRequest_Callback cb=NULL) :
		RequestBase<TopLevString>(val, key_pstr, help_pstr, vcb, cb), max_length(maxLength) {}

/*
	String(String* val, uint8_t maxLength, SOVA_FLASHSTRING_PTR key_pstr, SOVA_FLASHSTRING_PTR help_pstr, MenuRequest_Callback cb=NULL) :
		Request<String>(val, key_pstr, help_pstr, cb),  max_length(maxLength) {}
*/
	uint8_t max_length;

	virtual void showPrompt(Menu * callingMenu) ;
	virtual bool getValue(Menu * callingMenu, ::String* into);
	virtual void printPrefix(Menu * callingMenu, SUI::Mode::Selection curMode);

	virtual void printMetaInfo(SUI::Menu * callingMenu);

};





#if 0
class Char: public Request {
	public:
	Char(char * val, SOVA_FLASHSTRING_PTR key_pstr,
			SOVA_FLASHSTRING_PTR help_pstr, MenuRequest_Callback cb=NULL) :
		Request(key_pstr, help_pstr, cb), the_value(val) {}

	char * the_value;
	virtual bool showPromptAndGetValue(Menu * callingMenu);


};

class Bool: public Request {
	public:
	Bool(bool * val, SOVA_FLASHSTRING_PTR key_pstr,
			SOVA_FLASHSTRING_PTR help_pstr, MenuRequest_Callback cb=NULL) :
		Request(key_pstr, help_pstr, cb), the_value(val) {}

	bool * the_value;
	virtual bool showPromptAndGetValue(Menu * callingMenu);


};

class Long: public Request {
	public:
	Long(long int * val, SOVA_FLASHSTRING_PTR key_pstr,
			SOVA_FLASHSTRING_PTR help_pstr, MenuRequest_Callback cb=NULL) :
		Request(key_pstr, help_pstr, cb), the_value(val) {}

	long int * the_value;
	virtual bool showPromptAndGetValue(Menu * callingMenu);


};


class ULong: public Request {
	public:
	ULong(long unsigned int * val, SOVA_FLASHSTRING_PTR key_pstr,
			SOVA_FLASHSTRING_PTR help_pstr, MenuRequest_Callback cb=NULL) :
		Request(key_pstr, help_pstr, cb), the_value(val) {}

	long unsigned int * the_value;
	virtual bool showPromptAndGetValue(Menu * callingMenu);


};

class Float: public Request {
	public:
	Float(float * val, SOVA_FLASHSTRING_PTR key_pstr,
			SOVA_FLASHSTRING_PTR help_pstr, MenuRequest_Callback cb=NULL) :
		Request(key_pstr, help_pstr, cb), the_value(val) {}

	float * the_value;
	virtual bool showPromptAndGetValue(Menu * callingMenu);

};

class String: public Request {
public:
	String(char * val, uint8_t maxLen, SOVA_FLASHSTRING_PTR key_pstr,
			SOVA_FLASHSTRING_PTR help_pstr, MenuRequest_Callback cb=NULL) :
		Request(key_pstr, help_pstr, cb), the_value(val), max_length(maxLen) {}

	char * the_value;
	uint8_t max_length;
	virtual bool showPromptAndGetValue(Menu * callingMenu);

};

#endif


}
}
}


#endif /* SERIALUI_SRC_INCLUDES_MENUITEM_REQUEST_H_ */
