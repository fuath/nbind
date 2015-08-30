// This file is part of nbind, copyright (C) 2014-2015 BusFaster Ltd.
// Released under the MIT license, see LICENSE.

// This file is very similar to MethodSignature.h and AccessorSignature.h
// so modify them together.

#pragma once

#include "CallableSignature.h"

namespace nbind {

// Wrapper for all C++ functions with matching argument and return types.

template <typename ReturnType, typename... Args>
class FunctionSignature : public CallableSignature<FunctionSignature<ReturnType, Args...>, ReturnType, Args...> {

public:

	typedef struct {} Data;

	typedef ReturnType(*FunctionType)(Args...);

	typedef CallableSignature<FunctionSignature, ReturnType, Args...> Parent;

#ifdef BUILDING_NODE_EXTENSION
	static NAN_METHOD(call) {
		static constexpr decltype(args.Length()) arity = sizeof...(Args);

		NanScope();

		if(args.Length() != arity) {
//			printf("Wrong number of arguments to %s.%s: expected %ld, got %d.\n",getClassName(),getMethodName(),arity,args.Length());
			return(NanThrowError("Wrong number of arguments"));
		}

		if(!FunctionSignature::typesAreValid(args)) {
			return(NanThrowTypeError("Type mismatch"));
		}

		Status::clearError();

		try {
			auto result = Parent::CallWrapper::call(Parent::getFunction(args.Data()->IntegerValue()).func, args);

			const char *message = Status::getError();

			if(message != nullptr) return(NanThrowError(message));

			NanReturnValue(BindingType<ReturnType>::toWireType(std::move(result)));
		} catch(const std::exception &ex) {
			const char *message = Status::getError();

			if(message == nullptr) message = ex.what();

			return(NanThrowError(message));
		}
	}
#else
	static void call() {}
#endif // BUILDING_NODE_EXTENSION

};

} // namespace
