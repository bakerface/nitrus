/*
 * Copyright (c) 2012 Christopher M. Baker
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include "../include/Application.hpp"
#include "../include/rest/Rest.hpp"
using namespace nitrus;

#include <vector>

typedef int EntityId;
typedef std::vector<EntityId> EntityIdList;

class Controller {
public:
	static EntityIdList GetEntities() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		EntityIdList entities;
		entities.push_back(100);
		entities.push_back(101);

		return entities;
	}
	
	static EntityId GetEntityById(EntityId id) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return id;
	}
};

class JsonView {
public:
	template <typename Type> static void Single(const Rest::Router::RequestEventArgs& args, Type value, const Delegate<std::string (Type)>& transform) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		args.Client()->Begin("HTTP/1.1", 200, "OK").SendHeader("Content-Type", "application/json").Send(transform(value)).End();
	}
	
	template <typename Iterator, typename Type> static void Collection(const Rest::Router::RequestEventArgs& args, Iterator begin, Iterator end, const Delegate<std::string (Type)>& transform) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		args.Client()->Begin("HTTP/1.1", 200, "OK").SendHeader("Content-Type", "application/json").Send("[");

		for (Iterator i = begin; i != end; i++) {
			if (i != begin) {
				args.Client()->Send(",");
			}

			args.Client()->Send(transform(*i));
		}

		args.Client()->Send("]").End();
	}

	template <typename Container> static void Collection(const Rest::Router::RequestEventArgs& args, const Container& container, const Delegate<std::string (typename Container::value_type)>& transform) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Collection(args, container.begin(), container.end(), transform);
	}

	static std::string Transform(EntityId id) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return String::Format("{ \"Id\": %d }", id);
	}

	static void ReadEntities(const Rest::Router::RequestEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Collection(args, Controller::GetEntities(), delegate(Transform));
	}
	
	static void ReadEntity(const Rest::Router::RequestEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		Single(args, Controller::GetEntityById(args.Match<EntityId>("entityId")), delegate(Transform));
	}
};

/**
 * The entry point for the application.
 * @param argc The number of elements in the second parameter.
 * @param argv The array of arguments passed to this application from the system.
 * @return EXIT_SUCCESS if the application completed successfully or EXIT_FAILURE if an error occurred.
 */
int main(int argc, char** argv) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
	Application::Initialize(argc, argv);

	Rest::Router router(Application::GetParameter("--document-root", "www"));

	router.Configure("/entities")
		.Get(Rest::Router::RequestEventHandler(JsonView::ReadEntities));
		
	router.Configure("/entities/{entityId}")
		.Get(Rest::Router::RequestEventHandler(JsonView::ReadEntity));

	router.Bind(Application::GetParameter("--port", 9091));
	router.Listen();

	return Application::Run();
}
