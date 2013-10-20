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

#ifndef XML_HPP_
#define XML_HPP_

#include "../StackTrace.hpp"
#include "../state/StateMachine.hpp"
#include "../Event.hpp"
#include "../String.hpp"

#include <assert.h>
#include <map>
#include <stack>

namespace nitrus {

/**
 * A class that provides functionality for containing pieces of an XML attribute.
 */
class XmlAttribute {
private:
	std::string _namespace;
	std::string _name;
	std::string _value;

public:

	/**
	 * Creates a new empty XML attribute.
	 * This default constructor is provided to simplify use and should not be added to an XML element.
	 */
	XmlAttribute() : _namespace(), _name(), _value() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new XML attribute without a namespace.
	 *
	 * @param name The name of the attribute.
	 */
	XmlAttribute(const std::string& name) : _namespace(), _name(name), _value() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new XML attribute with a namespace.
	 *
	 * @param ns The namespace prefixing the name.
	 * @param name The name of the attribute.
	 */
	XmlAttribute(const std::string& ns, const std::string& name) : _namespace(ns), _name(name), _value() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new XML attribute with a namespace and a value.
	 *
	 * @param ns The namespace prefixing the name.
	 * @param name The name of the attribute.
	 * @param value The value of the attribute.
	 */
	XmlAttribute(const std::string& ns, const std::string& name, const std::string& value) : _namespace(ns), _name(name), _value(value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Gets the namespace that prefixes the name of the attribute.
	 *
	 * @return The attribute namespace.
	 */
	const std::string& Namespace() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _namespace;
	}

	/**
	 * Gets the name of the attribute.
	 *
	 * @return The attribute name.
	 */
	const std::string& Name() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _name;
	}

	/**
	 * Gets the value of the attribute.
	 *
	 * @return The attribute value.
	 */
	const std::string& Value() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _value;
	}

	/**
	 * Gets the value of the attribute.
	 *
	 * @return The attribute value.
	 */
	std::string& Value() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _value;
	}

	/**
	 * Sets the value of the attribute.
	 *
	 * @param value The attribute value.
	 * @return A reference to this attribute.
	 */
	XmlAttribute& Value(const std::string& value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_value = value;
		return *this;
	}

	/**
	 * Checks if this attribute exists.
	 *
	 * @return True if this attribute exists, false otherwise.
	 */
	bool Exists() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _name.empty() == false;
	}

	/**
	 * Deletes this XML attribute.
	 */
	virtual ~XmlAttribute() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}
};

/**
 * A class that provides functionality for containing pieces of an XML element.
 * XML documents are XML elements that contain sub-elements and XML attributes.
 */
class XmlElement {
public:
	typedef std::multimap<std::string, XmlElement> ElementCollection;
	typedef std::map<std::string, XmlAttribute> AttributeCollection;

	/**
	 * Replaces all XML reserved symbols in a string with their escaped abbreviation.
	 *
	 * @param value The string to escape.
	 * @return A new string with all reserved symbols escaped.
	 */
	static std::string Escape(const std::string& value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		std::string escaped = value;

		escaped = String::Replace(escaped, "&", "&amp;");
		escaped = String::Replace(escaped, "'", "&apos;");
		escaped = String::Replace(escaped, "\"", "&quot;");
		escaped = String::Replace(escaped, "<", "&lt;");
		escaped = String::Replace(escaped, ">", "&gt;");

		return escaped;
	}

	/**
	 * Replaces all escaped abbreviations with the XML reserved symbol.
	 *
	 * @param value The string to unescape.
	 * @return A new string with all escape sequences replaced with reserved symbols.
	 */
	static std::string Unescape(const std::string& value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		std::string unescaped = value;

		unescaped = String::Replace(unescaped, "&apos;", "'");
		unescaped = String::Replace(unescaped, "&quot;", "\"");
		unescaped = String::Replace(unescaped, "&lt;", "<");
		unescaped = String::Replace(unescaped, "&gt;", ">");
		unescaped = String::Replace(unescaped, "&amp;", "&");

		return unescaped;
	}

private:
	std::string _namespace;
	std::string _name;
	std::string _value;
	ElementCollection _elements;
	AttributeCollection _attributes;

private:
	static XmlElement NullElement;
	static XmlAttribute NullAttribute;

private:

	/**
	 * Gets the uniquely identifiable value that is used to internally reference an attribute or element given its namespace and name.
	 *
	 * @param ns The namespace of the attribute or element.
	 * @param name The name of the attribute or element.
	 * @return The unique key used to internally reference the attribute or element.
	 */
	static std::string Key(const std::string ns, const std::string& name) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return String::ToLowerCase(ns + ":" + name);
	}

	/**
	 * Appends the XML start element string to the buffer.
	 * The start element string consists of the element namespace, name, and attributes.
	 *
	 * @param element The element to append.
	 * @param buffer The buffer to append to.
	 */
	static void ToStartElementString(const XmlElement& element, std::string& buffer) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		buffer += "<";

		if (element.Namespace().empty() == false) {
			buffer += element.Namespace() + ":";
		}

		buffer += element.Name();

		for (AttributeCollection::const_iterator i = element.Attributes().begin(); i != element.Attributes().end(); i++) {
			buffer += " ";

			if (i->second.Namespace().empty() == false) {
				buffer += i->second.Namespace() + ":";
			}

			buffer += i->second.Name() + "='" + i->second.Value() + "'";
		}

		buffer += ">";
	}

	/**
	 * Appends the XML end element string to the buffer.
	 * The end element string consists of the element namespace and name.
	 *
	 * @param element The element to append.
	 * @param buffer The buffer to append to.
	 */
	static void ToEndElementString(const XmlElement& element, std::string& buffer) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		buffer += "</";

		if (element.Namespace().empty() == false) {
			buffer += element.Namespace() + ":";
		}

		buffer += element.Name() + ">";
	}

	/**
	 * Appends the XML element string to the buffer.
	 * The element string consists of the start element, value, and end element.
	 *
	 * @param element The element to append.
	 * @param buffer The buffer to append to.
	 */
	static void ToString(const XmlElement& element, std::string& buffer) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		ToStartElementString(element, buffer);

		for (ElementCollection::const_iterator i = element.Elements().begin(); i != element.Elements().end(); i++) {
			ToString(i->second, buffer);
		}

		buffer += Escape(element.Value());
		ToEndElementString(element, buffer);
	}

public:

	/**
	 * Creates a new empty XML element.
	 */
	XmlElement() : _namespace(), _name(), _value(), _elements(), _attributes() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new XML element without a namespace.
	 *
	 * @param name The name of the element.
	 */
	XmlElement(const std::string& name) : _namespace(), _name(name), _value(), _elements(), _attributes() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Creates a new XML element with a namespace.
	 *
	 * @param ns The namespace prefixing the name.
	 * @param name The name of the element.
	 */
	XmlElement(const std::string& ns, const std::string& name) : _namespace(ns), _name(name), _value(), _elements(), _attributes() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}

	/**
	 * Gets the namespace prefixing the element name.
	 *
	 * @return The element namespace.
	 */
	const std::string& Namespace() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _namespace;
	}

	/**
	 * Gets the name of the element.
	 *
	 * @return The element name.
	 */
	const std::string& Name() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _name;
	}

	/**
	 * Gets the value of the element.
	 * Typically, if an element has a value then it will not have sub-elements.
	 *
	 * @return The element value.
	 */
	const std::string& Value() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _value;
	}

	/**
	 * Gets the value of the element.
	 * Typically, if an element has a value then it will not have sub-elements.
	 *
	 * @return The element value.
	 */
	std::string& Value() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _value;
	}

	/**
	 * Sets the value of the element.
	 * Typically, if an element has a value then it will not have sub-elements.
	 *
	 * @param value The element value.
	 * @return A reference to this XML element.
	 */
	XmlElement& Value(const std::string& value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		_value = value;
		return *this;
	}

	/**
	 * Gets the collection of sub-elements contained by this element.
	 *
	 * @return The sub-elements.
	 */
	const ElementCollection& Elements() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _elements;
	}

	/**
	 * Gets the collection of sub-elements contained by this element.
	 *
	 * @return The sub-elements.
	 */
	ElementCollection& Elements() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _elements;
	}

	/**
	 * Adds a sub-element without a namespace.
	 *
	 * @param name The name of the element.
	 * @return A reference to the sub-element that was added.
	 */
	XmlElement& Add(const std::string& name) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return Add("", name);
	}

	/**
	 * Adds a sub-element with a namespace.
	 *
	 * @param ns The namespace prefixing the element name.
	 * @param name The name of the element.
	 * @return A reference to the sub-element that was added.
	 */
	XmlElement& Add(const std::string& ns, const std::string& name) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _elements.insert(std::make_pair(Key(ns, name), XmlElement(ns, name)))->second;
	}

	/**
	 * Gets a reference to a sub-element.
	 * If the element does not exist, a reference to an empty XML element is returned.
	 *
	 * @param name The name of the sub-element.
	 * @return A reference to the sub-element if it exists, or a reference to an empty element otherwise.
	 */
	const XmlElement& Element(const std::string& name) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return Element("", name);
	}

	/**
	 * Gets a reference to a sub-element.
	 * If the element does not exist, a new sub-element is added and its reference is returned.
	 *
	 * @param name The name of the sub-element.
	 * @return A reference to the sub-element.
	 */
	XmlElement& Element(const std::string& name) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return Element("", name);
	}

	/**
	 * Gets a reference to a sub-element.
	 * If the element does not exist, a reference to an empty XML element is returned.
	 *
	 * @param ns The namespace prefixing the name of the sub-element.
	 * @param name The name of the sub-element.
	 * @return A reference to the sub-element if it exists, or a reference to an empty element otherwise.
	 */
	const XmlElement& Element(const std::string& ns, const std::string& name) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		ElementCollection::const_iterator i = _elements.find(Key(ns, name));

		if (i == _elements.end()) {
			return NullElement;
		}

		return i->second;
	}

	/**
	 * Gets a reference to a sub-element.
	 * If the element does not exist, a new sub-element is added and its reference is returned.
	 *
	 * @param ns The namespace prefixing the name of the sub-element.
	 * @param name The name of the sub-element.
	 * @return A reference to the sub-element.
	 */
	XmlElement& Element(const std::string& ns, const std::string& name) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		std::string key = Key(ns, name);
		ElementCollection::iterator i = _elements.find(key);

		if (i == _elements.end()) {
			return Add(ns, name);
		}

		return i->second;
	}

	/**
	 * Gets the collection of attributes for this element.
	 *
	 * @return The attributes.
	 */
	const AttributeCollection& Attributes() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _attributes;
	}

	/**
	 * Gets the collection of attributes for this element.
	 *
	 * @return The attributes.
	 */
	AttributeCollection& Attributes() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _attributes;
	}

	/**
	 * Gets a reference to an attribute.
	 * If the attribute does not exist, a reference to an empty XML attribute is returned.
	 *
	 * @param name The name of the attribute.
	 * @return A reference to the attribute if it exists, or a reference to an empty attribute otherwise.
	 */
	const XmlAttribute& Attribute(const std::string& name) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return Attribute("", name);
	}

	/**
	 * Gets a reference to an attribute.
	 * If the attribute does not exist, a new attribute is added and its reference is returned.
	 *
	 * @param name The name of the attribute.
	 * @return A reference to the attribute.
	 */
	XmlAttribute& Attribute(const std::string& name) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return Attribute("", name);
	}

	/**
	 * Gets a reference to an attribute.
	 * If the attribute does not exist, a reference to an empty XML attribute is returned.
	 *
	 * @param ns The namespace prefixing the name of the attribute.
	 * @param name The name of the attribute.
	 * @return A reference to the attribute if it exists, or a reference to an empty attribute otherwise.
	 */
	const XmlAttribute& Attribute(const std::string& ns, const std::string& name) const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		AttributeCollection::const_iterator i = _attributes.find(Key(ns, name));

		if (i == _attributes.end()) {
			return NullAttribute;
		}

		return i->second;
	}

	/**
	 * Gets a reference to an attribute.
	 * If the attribute does not exist, a new attribute is added and its reference is returned.
	 *
	 * @param ns The namespace prefixing the name of the attribute.
	 * @param name The name of the attribute.
	 * @return A reference to the attribute.
	 */
	XmlAttribute& Attribute(const std::string& ns, const std::string& name) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		std::string key = Key(ns, name);
		AttributeCollection::iterator i = _attributes.find(key);

		if (i == _attributes.end()) {
			return _attributes.insert(std::make_pair(key, XmlAttribute(ns, name))).first->second;
		}

		return i->second;
	}

	/**
	 * Checks if this element exists.
	 *
	 * @return True if this element exists, false otherwise.
	 */
	bool Exists() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		return _name.empty() == false;
	}

	/**
	 * Gets the XML start element string for this element.
	 * The start element string consists of the element namespace, name, and attributes.
	 *
	 * @return The start element string.
	 */
	std::string ToStartElementString() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		std::string buffer;
		ToStartElementString(*this, buffer);
		return buffer;
	}

	/**
	 * Gets the XML end element string for this element.
	 * The end element string consists of the element namespace and name.
	 *
	 * @return The end element string.
	 */
	std::string ToEndElementString() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		std::string buffer;
		ToEndElementString(*this, buffer);
		return buffer;
	}

	/**
	 * Gets the XML element string for this element.
	 * The element string consists of the start element, value, and end element.
	 *
	 * @return The element string.
	 */
	std::string ToString() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		std::string buffer;
		ToString(*this, buffer);
		return buffer;
	}

	/**
	 * Deletes this XML element.
	 */
	virtual ~XmlElement() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

	}
};

/**
 * A class that provides functionality for parsing and creating XML documents.
 */
class Xml {
public:

	/**
	 * A class that provides functionality for progressively parsing XML documents.
	 * Individual events are triggered for start elements, attribute names, attribute values, element data, and end elements.
	 */
	class ProgressiveParser {
	public:

		/**
		 * A class that encapsulates a start element inside an XML document.
		 */
		class StartElementReceivedEventArgs : public EventArgs {
		private:
			std::string _namespace;
			std::string _name;

		public:

			/**
			 * Creates a new start element received event argument.
			 *
			 * @param ns The namespace prefixing the element name.
			 * @param name The name of the element.
			 */
			StartElementReceivedEventArgs(const std::string& ns, const std::string& name) : _namespace(ns), _name(name) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}

			/**
			 * Gets the namespace prefixing the element name.
			 *
			 * @return The namespace.
			 */
			const std::string& Namespace() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return _namespace;
			}

			/**
			 * Gets the name of the element.
			 *
			 * @return The element name.
			 */
			const std::string& Name() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return _name;
			}

			/**
			 * Deletes this event argument.
			 */
			virtual ~StartElementReceivedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}
		};

		typedef EventHandler<const StartElementReceivedEventArgs&> StartElementReceivedEventHandler;
		typedef Event<const StartElementReceivedEventArgs&> StartElementReceivedEvent;

		/**
		 * A class that encapsulates an end element inside an XML document.
		 */
		class EndElementReceivedEventArgs : public EventArgs {
		private:
			std::string _namespace;
			std::string _name;

		public:

			/**
			 * Creates a new end element received event argument.
			 *
			 * @param ns The namespace prefixing the element name.
			 * @param name The name of the element.
			 */
			EndElementReceivedEventArgs(const std::string& ns, const std::string& name) : _namespace(ns), _name(name) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}

			/**
			 * Gets the namespace prefixing the element name.
			 *
			 * @return The namespace.
			 */
			const std::string& Namespace() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return _namespace;
			}

			/**
			 * Gets the name of the element.
			 *
			 * @return The element name.
			 */
			const std::string& Name() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return _name;
			}

			/**
			 * Deletes this event argument.
			 */
			virtual ~EndElementReceivedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}
		};

		typedef EventHandler<const EndElementReceivedEventArgs&> EndElementReceivedEventHandler;
		typedef Event<const EndElementReceivedEventArgs&> EndElementReceivedEvent;

		/**
		 * A class that encapsulates the name of an attribute inside an XML document.
		 */
		class AttributeNameReceivedEventArgs : public EventArgs {
		private:
			std::string _namespace;
			std::string _name;

		public:

			/**
			 * Creates a new attribute name received event argument.
			 *
			 * @param ns The namespace prefixing the attribute name.
			 * @param name The name of the attribute.
			 */
			AttributeNameReceivedEventArgs(const std::string& ns, const std::string& name) : _namespace(ns), _name(name) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}

			/**
			 * Gets the namespace prefixing the attribute name.
			 *
			 * @return The attribute namespace.
			 */
			const std::string& Namespace() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return _namespace;
			}

			/**
			 * Gets the name of the attribute.
			 *
			 * @return The attribute name.
			 */
			const std::string& Name() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return _name;
			}

			/**
			 * Deletes this event argument.
			 */
			virtual ~AttributeNameReceivedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}
		};

		typedef EventHandler<const AttributeNameReceivedEventArgs&> AttributeNameReceivedEventHandler;
		typedef Event<const AttributeNameReceivedEventArgs&> AttributeNameReceivedEvent;

		/**
		 * A class that encapsulates the value of an attribute inside an XML document.
		 */
		class AttributeValueReceivedEventArgs : public EventArgs {
		private:
			std::string _value;

		public:

			/**
			 * Creates a new attribute value received event argument.
			 *
			 * @param value The attribute value.
			 */
			AttributeValueReceivedEventArgs(const std::string& value) : _value(value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}

			/**
			 * Gets the value of an attribute.
			 *
			 * @return The attribute value.
			 */
			const std::string& Value() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return _value;
			}

			/**
			 * Deletes this event argument.
			 */
			virtual ~AttributeValueReceivedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}
		};

		typedef EventHandler<const AttributeValueReceivedEventArgs&> AttributeValueReceivedEventHandler;
		typedef Event<const AttributeValueReceivedEventArgs&> AttributeValueReceivedEvent;

		/**
		 * A class that encapsulates the data of an element inside an XML document.
		 */
		class DataReceivedEventArgs : public EventArgs {
		private:
			std::string _data;

		public:

			/**
			 * Creates a new data received event argument.
			 *
			 * @param data The element data.
			 */
			DataReceivedEventArgs(const std::string& data) : _data(data) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}

			/**
			 * Gets the data of an element.
			 *
			 * @return The element data.
			 */
			const std::string& Data() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return _data;
			}

			/**
			 * Deletes this event argument.
			 */
			virtual ~DataReceivedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}
		};

		typedef EventHandler<const DataReceivedEventArgs&> DataReceivedEventHandler;
		typedef Event<const DataReceivedEventArgs&> DataReceivedEvent;

	private:
		enum State {
			State_OpenElement,
			State_IgnoreDeclaration,
			State_CloseElement,
			State_OptionalSlashOrQuestionAfterOpenElement,
			State_StartElement,
			State_EndElement,
			State_OptionalAttribute,
			State_AttributeName,
			State_OptionalSlashAfterAttributes,
			State_ImmediateEndElement,
			State_OptionalOpenElement,
			State_ElementData,
			State_AttributeAssignment,
			State_AttributeValueDetermineQuotes,
			State_StartAttributeValueSingleQuotes,
			State_StartAttributeValueDoubleQuotes,
			State_EndAttributeValueSingleQuotes,
			State_EndAttributeValueDoubleQuotes,
			State_OptionalWhitespaceBeforeAttribute
		};

		enum Trigger {
			Trigger_Append,
			Trigger_OpenElementReceived,
			Trigger_DeclarationReceived,
			Trigger_OptionalSlashPresent,
			Trigger_OptionalQuestionPresent,
			Trigger_OptionalSlashNotPresent,
			Trigger_ElementNameReceived,
			Trigger_OptionalAttributePresent,
			Trigger_OptionalAttributeNotPresent,
			Trigger_OptionalOpenElementPresent,
			Trigger_OptionalOpenElementNotPresent,
			Trigger_ElementDataReceived,
			Trigger_CloseElementReceived,
			Trigger_AttributeNameReceived,
			Trigger_AttributeAssignmentReceived,
			Trigger_SingleQuotesReceived,
			Trigger_DoubleQuotesReceived,
			Trigger_AttributeValueReceived,
			Trigger_WhitespaceReceived,
			Trigger_ImmediateEndElementReceived
		};

	private:
		StateMachine<State, Trigger> _stateMachine;
		std::string _buffer;
		std::vector<std::string> _elementNamespaceAndName;
		StartElementReceivedEvent _startElementReceived;
		EndElementReceivedEvent _endElementReceived;
		AttributeNameReceivedEvent _attributeNameReceived;
		AttributeValueReceivedEvent _attributeValueReceived;
		DataReceivedEvent _dataReceived;

	private:
		template <typename Signature> friend class Delegate;

		/**
		 * Called when the open element should attempt to be parsed.
		 * If it is successfully parsed, the state machine moves to the next state.
		 * If partial data was received, the state machine waits for more data.
		 */
		void State_OpenElement_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			if (_buffer.empty() == false) {
				if (_buffer.at(0) == '<') {
					_buffer.erase(0, 1);
					_stateMachine.Fire(Trigger_OpenElementReceived);
				}
			}
		}

		/**
		 * Called when the parser needs to look ahead to determine if this is an XML declaration, start element, or end element.
		 * If it could successfully look ahead, the state machine moves to the next state.
		 * If partial data was received, the state machine waits for more data.
		 */
		void State_OptionalSlashOrQuestionAfterOpenElement_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			if (_buffer.empty() == false) {
				if (_buffer.at(0) == '/') {
					_stateMachine.Fire(Trigger_OptionalSlashPresent);
				}
				else if (_buffer.at(0) == '?') {
					_stateMachine.Fire(Trigger_OptionalQuestionPresent);
				}
				else {
					_stateMachine.Fire(Trigger_OptionalSlashNotPresent);
				}
			}
		}

		/**
		 * Called when the XML declaration should attempt to be parsed.
		 * If it is successfully parsed, the state machine moves to the next state.
		 * If partial data was received, the state machine waits for more data.
		 */
		void State_IgnoreDeclaration_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			size_t end = _buffer.find("?>");

			if (end != std::string::npos) {
				_buffer.erase(0, end + 2);
				_stateMachine.Fire(Trigger_DeclarationReceived);
			}
		}

		/**
		 * Called when the end element should attempt to be parsed.
		 * If it is successfully parsed, the state machine moves to the next state.
		 * If partial data was received, the state machine waits for more data.
		 */
		void State_EndElement_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			size_t end = _buffer.find('>');

			if (end != std::string::npos) {
				std::vector<std::string> namespaceAndName = String::Split(_buffer.substr(1, end - 1), ':');
				_buffer.erase(0, end + 1);

				if (namespaceAndName.size() == 1) {
					_endElementReceived(EndElementReceivedEventArgs("", namespaceAndName[0]), this);
				}
				else if (namespaceAndName.size() == 2) {
					_endElementReceived(EndElementReceivedEventArgs(namespaceAndName[0], namespaceAndName[1]), this);
				}

				_stateMachine.Fire(Trigger_ElementNameReceived);
			}
		}

		/**
		 * Called when the start element should attempt to be parsed.
		 * If it is successfully parsed, the state machine moves to the next state.
		 * If partial data was received, the state machine waits for more data.
		 */
		void State_StartElement_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			size_t end = _buffer.find_first_of("/> \t\r\n");

			if (end != std::string::npos) {
				_elementNamespaceAndName = String::Split(_buffer.substr(0, end), ':');
				_buffer.erase(0, end);

				if (_elementNamespaceAndName.size() == 1) {
					_startElementReceived(StartElementReceivedEventArgs("", _elementNamespaceAndName[0]), this);
				}
				else if (_elementNamespaceAndName.size() == 2) {
					_startElementReceived(StartElementReceivedEventArgs(_elementNamespaceAndName[0], _elementNamespaceAndName[1]), this);
				}

				_stateMachine.Fire(Trigger_ElementNameReceived);
			}
		}

		/**
		 * Called when the optional whitespace should attempt to be parsed and ignored.
		 * If it is successfully parsed, the state machine moves to the next state.
		 * If partial data was received, the state machine waits for more data.
		 */
		void State_OptionalWhitespaceBeforeAttribute_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			size_t end = _buffer.find_first_not_of("? \t\r\n");

			if (end != std::string::npos) {
				_buffer.erase(0, end);
				_stateMachine.Fire(Trigger_WhitespaceReceived);
			}
		}

		/**
		 * Called when the parser needs to look ahead to determine if another attribute needs to be parsed.
		 * If it could successfully look ahead, the state machine moves to the next state.
		 * If partial data was received, the state machine waits for more data.
		 */
		void State_OptionalAttribute_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			if (_buffer.empty() == false) {
				if (_buffer.at(0) == '/' || _buffer.at(0) == '>') {
					_stateMachine.Fire(Trigger_OptionalAttributeNotPresent);
				}
				else {
					_stateMachine.Fire(Trigger_OptionalAttributePresent);
				}
			}
		}

		/**
		 * Called when the attribute name should attempt to be parsed.
		 * If it is successfully parsed, the state machine moves to the next state.
		 * If partial data was received, the state machine waits for more data.
		 */
		void State_AttributeName_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			size_t end = _buffer.find('=');

			if (end != std::string::npos) {
				size_t start = _buffer.find_first_not_of(" \t\r\n");
				std::vector<std::string> namespaceAndName = String::Split(_buffer.substr(0, end), ':');
				_buffer.erase(0, end);

				if (namespaceAndName.size() == 1) {
					_attributeNameReceived(AttributeNameReceivedEventArgs("", namespaceAndName[0]), this);
				}
				else if (namespaceAndName.size() == 2) {
					_attributeNameReceived(AttributeNameReceivedEventArgs(namespaceAndName[0], namespaceAndName[1]), this);
				}

				_stateMachine.Fire(Trigger_AttributeNameReceived);
			}
		}

		/**
		 * Called when the attribute assignment operator should attempt to be parsed.
		 * If it is successfully parsed, the state machine moves to the next state.
		 * If partial data was received, the state machine waits for more data.
		 */
		void State_AttributeAssignment_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			if (_buffer.empty() == false) {
				if (_buffer.at(0) == '=') {
					_buffer.erase(0, 1);
					_stateMachine.Fire(Trigger_AttributeAssignmentReceived);
				}
			}
		}

		/**
		 * Called when the attribute value quoting type needs to be parsed.
		 * If it is successfully parsed, the state machine moves to the next state.
		 * If partial data was received, the state machine waits for more data.
		 */
		void State_AttributeValueDetermineQuotes_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			if (_buffer.empty() == false) {
				if (_buffer.at(0) == '"') {
					_buffer.erase(0, 1);
					_stateMachine.Fire(Trigger_DoubleQuotesReceived);
				}
				else if (_buffer.at(0) == '\'') {
					_buffer.erase(0, 1);
					_stateMachine.Fire(Trigger_SingleQuotesReceived);
				}
			}
		}

		/**
		 * Called when the single-quoted attribute value should attempt to be parsed.
		 * If it is successfully parsed, the state machine moves to the next state.
		 * If partial data was received, the state machine waits for more data.
		 */
		void State_StartAttributeValueSingleQuotes_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			size_t end = _buffer.find('\'');

			if (end != std::string::npos) {
				std::string value = _buffer.substr(0, end);
				_buffer.erase(0, end);

				_attributeValueReceived(AttributeValueReceivedEventArgs(value), this);
				_stateMachine.Fire(Trigger_AttributeValueReceived);
			}
		}

		/**
		 * Called when the double-quoted attribute value should attempt to be parsed.
		 * If it is successfully parsed, the state machine moves to the next state.
		 * If partial data was received, the state machine waits for more data.
		 */
		void State_StartAttributeValueDoubleQuotes_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			size_t end = _buffer.find('"');

			if (end != std::string::npos) {
				std::string value = _buffer.substr(0, end);
				_buffer.erase(0, end);

				_attributeValueReceived(AttributeValueReceivedEventArgs(value), this);
				_stateMachine.Fire(Trigger_AttributeValueReceived);
			}
		}

		/**
		 * Called when the end of the single-quoted attribute value should attempt to be parsed.
		 * If it is successfully parsed, the state machine moves to the next state.
		 * If partial data was received, the state machine waits for more data.
		 */
		void State_EndAttributeValueSingleQuotes_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			if (_buffer.empty() == false) {
				if (_buffer.at(0) == '\'') {
					_buffer.erase(0, 1);
					_stateMachine.Fire(Trigger_SingleQuotesReceived);
				}
			}
		}

		/**
		 * Called when the end of the double-quoted attribute value should attempt to be parsed.
		 * If it is successfully parsed, the state machine moves to the next state.
		 * If partial data was received, the state machine waits for more data.
		 */
		void State_EndAttributeValueDoubleQuotes_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			if (_buffer.empty() == false) {
				if (_buffer.at(0) == '"') {
					_buffer.erase(0, 1);
					_stateMachine.Fire(Trigger_DoubleQuotesReceived);
				}
			}
		}

		/**
		 * Called when the parser needs to look ahead to determine if an immediate end for a start element exists.
		 * If it could successfully look ahead, the state machine moves to the next state.
		 * If partial data was received, the state machine waits for more data.
		 */
		void State_OptionalSlashAfterAttributes_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			if (_buffer.empty() == false) {
				if (_buffer.at(0) == '/') {
					_stateMachine.Fire(Trigger_OptionalSlashPresent);
				}
				else {
					_stateMachine.Fire(Trigger_OptionalSlashNotPresent);
				}
			}
		}

		/**
		 * Called when the immediate end for a start element should attempt to be parsed.
		 * If it is successfully parsed, the state machine moves to the next state.
		 * If partial data was received, the state machine waits for more data.
		 */
		void State_ImmediateEndElement_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			if (_buffer.size() >= 2) {
				if (_buffer.substr(0, 2) == "/>") {
					_buffer.erase(0, 2);

					if (_elementNamespaceAndName.size() == 1) {
						_endElementReceived(EndElementReceivedEventArgs("", _elementNamespaceAndName[0]), this);
					}
					else if (_elementNamespaceAndName.size() == 2) {
						_endElementReceived(EndElementReceivedEventArgs(_elementNamespaceAndName[0], _elementNamespaceAndName[1]), this);
					}

					_stateMachine.Fire(Trigger_ImmediateEndElementReceived);
				}
			}
		}

		/**
		 * Called when the close element should attempt to be parsed.
		 * If it is successfully parsed, the state machine moves to the next state.
		 * If partial data was received, the state machine waits for more data.
		 */
		void State_CloseElement_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			if (_buffer.empty() == false) {
				if (_buffer.at(0) == '>') {
					_buffer.erase(0, 1);
					_stateMachine.Fire(Trigger_CloseElementReceived);
				}
			}
		}

		/**
		 * Called when the parser needs to look ahead to determine if an open element exists.
		 * If it could successfully look ahead, the state machine moves to the next state.
		 * If partial data was received, the state machine waits for more data.
		 */
		void State_OptionalOpenElement_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			if (_buffer.empty() == false) {
				if (_buffer.at(0) == '<') {
					_stateMachine.Fire(Trigger_OptionalOpenElementPresent);
				}
				else {
					_stateMachine.Fire(Trigger_OptionalOpenElementNotPresent);
				}
			}
		}

		/**
		 * Called when the element data should attempt to be parsed.
		 * If it is successfully parsed, the state machine moves to the next state.
		 * If partial data was received, the state machine waits for more data.
		 */
		void State_ElementData_Entered() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			size_t end = _buffer.find('<');
			std::string data = _buffer.substr(0, end);
			_buffer.erase(0, end);

			_dataReceived(DataReceivedEventArgs(XmlElement::Unescape(data)), this);

			if (end != std::string::npos) {
				_stateMachine.Fire(Trigger_ElementDataReceived);
			}
		}

	public:

		/**
		 * Creates a new progressive XML parser.
		 */
		ProgressiveParser() : _stateMachine(State_OpenElement), _buffer(), _elementNamespaceAndName(), _startElementReceived(), _endElementReceived(), _attributeNameReceived(), _attributeValueReceived(), _dataReceived() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_stateMachine.Configure(State_OpenElement)
				.OnEntry(delegate(&ProgressiveParser::State_OpenElement_Entered, this))
				.Permit(Trigger_Append, State_OpenElement)
				.Permit(Trigger_OpenElementReceived, State_OptionalSlashOrQuestionAfterOpenElement);

			_stateMachine.Configure(State_OptionalSlashOrQuestionAfterOpenElement)
				.OnEntry(delegate(&ProgressiveParser::State_OptionalSlashOrQuestionAfterOpenElement_Entered, this))
				.Permit(Trigger_Append, State_OptionalSlashOrQuestionAfterOpenElement)
				.Permit(Trigger_OptionalQuestionPresent, State_IgnoreDeclaration)
				.Permit(Trigger_OptionalSlashPresent, State_EndElement)
				.Permit(Trigger_OptionalSlashNotPresent, State_StartElement);

			_stateMachine.Configure(State_IgnoreDeclaration)
				.OnEntry(delegate(&ProgressiveParser::State_IgnoreDeclaration_Entered, this))
				.Permit(Trigger_Append, State_IgnoreDeclaration)
				.Permit(Trigger_DeclarationReceived, State_OpenElement);

			_stateMachine.Configure(State_EndElement)
				.OnEntry(delegate(&ProgressiveParser::State_EndElement_Entered, this))
				.Permit(Trigger_Append, State_EndElement)
				.Permit(Trigger_ElementNameReceived, State_OpenElement);

			_stateMachine.Configure(State_StartElement)
				.OnEntry(delegate(&ProgressiveParser::State_StartElement_Entered, this))
				.Permit(Trigger_Append, State_StartElement)
				.Permit(Trigger_ElementNameReceived, State_OptionalWhitespaceBeforeAttribute);

			_stateMachine.Configure(State_OptionalWhitespaceBeforeAttribute)
				.OnEntry(delegate(&ProgressiveParser::State_OptionalWhitespaceBeforeAttribute_Entered, this))
				.Permit(Trigger_Append, State_OptionalWhitespaceBeforeAttribute)
				.Permit(Trigger_WhitespaceReceived, State_OptionalAttribute);

			_stateMachine.Configure(State_OptionalAttribute)
				.OnEntry(delegate(&ProgressiveParser::State_OptionalAttribute_Entered, this))
				.Permit(Trigger_Append, State_OptionalAttribute)
				.Permit(Trigger_OptionalAttributePresent, State_AttributeName)
				.Permit(Trigger_OptionalAttributeNotPresent, State_OptionalSlashAfterAttributes);

			_stateMachine.Configure(State_AttributeName)
				.OnEntry(delegate(&ProgressiveParser::State_AttributeName_Entered, this))
				.Permit(Trigger_Append, State_AttributeName)
				.Permit(Trigger_AttributeNameReceived, State_AttributeAssignment);

			_stateMachine.Configure(State_AttributeAssignment)
				.OnEntry(delegate(&ProgressiveParser::State_AttributeAssignment_Entered, this))
				.Permit(Trigger_Append, State_AttributeAssignment)
				.Permit(Trigger_AttributeAssignmentReceived, State_AttributeValueDetermineQuotes);

			_stateMachine.Configure(State_AttributeValueDetermineQuotes)
				.OnEntry(delegate(&ProgressiveParser::State_AttributeValueDetermineQuotes_Entered, this))
				.Permit(Trigger_Append, State_AttributeValueDetermineQuotes)
				.Permit(Trigger_SingleQuotesReceived, State_StartAttributeValueSingleQuotes)
				.Permit(Trigger_DoubleQuotesReceived, State_StartAttributeValueDoubleQuotes);

			_stateMachine.Configure(State_StartAttributeValueSingleQuotes)
				.OnEntry(delegate(&ProgressiveParser::State_StartAttributeValueSingleQuotes_Entered, this))
				.Permit(Trigger_Append, State_StartAttributeValueSingleQuotes)
				.Permit(Trigger_AttributeValueReceived, State_EndAttributeValueSingleQuotes);

			_stateMachine.Configure(State_StartAttributeValueDoubleQuotes)
				.OnEntry(delegate(&ProgressiveParser::State_StartAttributeValueDoubleQuotes_Entered, this))
				.Permit(Trigger_Append, State_StartAttributeValueDoubleQuotes)
				.Permit(Trigger_AttributeValueReceived, State_EndAttributeValueDoubleQuotes);

			_stateMachine.Configure(State_EndAttributeValueSingleQuotes)
				.OnEntry(delegate(&ProgressiveParser::State_EndAttributeValueSingleQuotes_Entered, this))
				.Permit(Trigger_Append, State_EndAttributeValueSingleQuotes)
				.Permit(Trigger_SingleQuotesReceived, State_OptionalWhitespaceBeforeAttribute);

			_stateMachine.Configure(State_EndAttributeValueDoubleQuotes)
				.OnEntry(delegate(&ProgressiveParser::State_EndAttributeValueDoubleQuotes_Entered, this))
				.Permit(Trigger_Append, State_EndAttributeValueDoubleQuotes)
				.Permit(Trigger_DoubleQuotesReceived, State_OptionalWhitespaceBeforeAttribute);

			_stateMachine.Configure(State_OptionalSlashAfterAttributes)
				.OnEntry(delegate(&ProgressiveParser::State_OptionalSlashAfterAttributes_Entered, this))
				.Permit(Trigger_Append, State_OptionalSlashAfterAttributes)
				.Permit(Trigger_OptionalSlashPresent, State_ImmediateEndElement)
				.Permit(Trigger_OptionalSlashNotPresent, State_CloseElement);

			_stateMachine.Configure(State_ImmediateEndElement)
				.OnEntry(delegate(&ProgressiveParser::State_ImmediateEndElement_Entered, this))
				.Permit(Trigger_Append, State_ImmediateEndElement)
				.Permit(Trigger_ImmediateEndElementReceived, State_OpenElement);

			_stateMachine.Configure(State_CloseElement)
				.OnEntry(delegate(&ProgressiveParser::State_CloseElement_Entered, this))
				.Permit(Trigger_Append, State_CloseElement)
				.Permit(Trigger_CloseElementReceived, State_OptionalOpenElement);

			_stateMachine.Configure(State_OptionalOpenElement)
				.OnEntry(delegate(&ProgressiveParser::State_OptionalOpenElement_Entered, this))
				.Permit(Trigger_Append, State_OptionalOpenElement)
				.Permit(Trigger_OptionalOpenElementPresent, State_OpenElement)
				.Permit(Trigger_OptionalOpenElementNotPresent, State_ElementData);

			_stateMachine.Configure(State_ElementData)
				.OnEntry(delegate(&ProgressiveParser::State_ElementData_Entered, this))
				.Permit(Trigger_Append, State_ElementData)
				.Permit(Trigger_ElementDataReceived, State_OpenElement);
		}

		/**
		 * Appends partial data to the parsing buffer so that it may be parsed.
		 *
		 * @param value The data to append.
		 * @return A reference to this parser.
		 */
		ProgressiveParser& Append(const std::string& value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_buffer.append(value);
			_stateMachine.Fire(Trigger_Append);

			return *this;
		}

		/**
		 * Gets the buffer of data this still needs to be parsed.
		 *
		 * @return The parsing buffer.
		 */
		const std::string& Buffer() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _buffer;
		}

		/**
		 * The event used to notify listeners that a start element has been received.
		 *
		 * @return The event.
		 */
		StartElementReceivedEvent& StartElementReceived() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _startElementReceived;
		}

		/**
		 * The event used to notify listeners that an end element has been received.
		 *
		 * @return The event.
		 */
		EndElementReceivedEvent& EndElementReceived() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _endElementReceived;
		}

		/**
		 * The event used to notify listeners that an attribute name has been received.
		 *
		 * @return The event.
		 */
		AttributeNameReceivedEvent& AttributeNameReceived() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _attributeNameReceived;
		}

		/**
		 * The event used to notify listeners that an attribute value has been received.
		 *
		 * @return The event.
		 */
		AttributeValueReceivedEvent& AttributeValueReceived() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _attributeValueReceived;
		}

		/**
		 * The event used to notify listeners that element data has been received.
		 *
		 * @return The event.
		 */
		DataReceivedEvent& DataReceived() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _dataReceived;
		}

		/**
		 * Deletes this progressive parser.
		 */
		virtual ~ProgressiveParser() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

public:

	/**
	 * A class that provides functionality for parsing XML documents into XmlElements and XmlAttributes.
	 * A single event is triggered when the document has been completely parsed.
	 */
	class DocumentParser {
	public:

		/**
		 * An invalid format exception that occurs when an XML document was invalid.
		 */
		class InvalidFormatException : public std::runtime_error {
		public:

			/**
			 * Creates a new invalid format exception.
			 */
			InvalidFormatException() : std::runtime_error(__METHOD__) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}

			/**
			 * Deletes this invalid format exception.
			 */
			virtual ~InvalidFormatException() throw() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}
		};

		/**
		 * A class that encapsulates a parsed document.
		 */
		class DocumentParsedEventArgs : public EventArgs {
		private:
			XmlElement _document;

		public:

			/**
			 * Creates a new document parsed event argument.
			 *
			 * @param document The document that was parsed.
			 */
			DocumentParsedEventArgs(const XmlElement& document) : _document(document) { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}

			/**
			 * Gets the document.
			 *
			 * @return The parsed document.
			 */
			const XmlElement& Document() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
				return _document;
			}

			/**
			 * Deletes this event argument.
			 */
			virtual ~DocumentParsedEventArgs() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

			}
		};

		typedef EventHandler<const DocumentParsedEventArgs&> DocumentParsedEventHandler;
		typedef Event<const DocumentParsedEventArgs&> DocumentParsedEvent;

	private:
		ProgressiveParser _parser;
		XmlElement _root;
		std::stack<XmlElement*> _stack;
		std::string _attributeNamespace;
		std::string _attributeName;
		DocumentParsedEvent _documentParsed;

	private:
		template <typename Signature> friend class Delegate;

		/**
		 * Called when a start element has been received.
		 *
		 * @param args The event arguments.
		 * @param sender The sender of the event.
		 */
		void OnStartElementReceived(const ProgressiveParser::StartElementReceivedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_stack.push(&_stack.top()->Add(args.Namespace(), args.Name()));
		}

		/**
		 * Called when an attribute name has been received.
		 *
		 * @param args The event arguments.
		 * @param sender The sender of the event.
		 */
		void OnAttributeNameReceived(const ProgressiveParser::AttributeNameReceivedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_attributeNamespace = args.Namespace();
			_attributeName = args.Name();
		}

		/**
		 * Called when an attribute value has been received.
		 *
		 * @param args The event arguments.
		 * @param sender The sender of the event.
		 */
		void OnAttributeValueReceived(const ProgressiveParser::AttributeValueReceivedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_stack.top()->Attribute(_attributeNamespace, _attributeName).Value(args.Value());
		}

		/**
		 * Called when a element data has been received.
		 *
		 * @param args The event arguments.
		 * @param sender The sender of the event.
		 */
		void OnDataReceived(const ProgressiveParser::DataReceivedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_stack.top()->Value(args.Data());
		}

		/**
		 * Called when an end element has been received.
		 * If the name of this element would cause the document to be unbalanced, an InvalidFormatException is thrown.
		 * If this element is the root level element for the document, the parsed event is triggered.
		 *
		 * @param args The event arguments.
		 * @param sender The sender of the event.
		 */
		void OnEndElementReceived(const ProgressiveParser::EndElementReceivedEventArgs& args, void* sender) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			XmlElement* top = _stack.top();
			_stack.pop();

			if (top->Namespace() != args.Namespace() || top->Name() != args.Name()) {
				throw InvalidFormatException();
			}

			if (_stack.top() == &_root) {
				_documentParsed(DocumentParsedEventArgs(*top), this);
				_root = XmlElement();
			}
		}

	public:
		/**
		 * Creates a new XML document parser.
		 */
		DocumentParser() : _parser(), _root(), _stack(), _attributeNamespace(), _attributeName(), _documentParsed() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_parser.StartElementReceived() += delegate(&DocumentParser::OnStartElementReceived, this);
			_parser.AttributeNameReceived() += delegate(&DocumentParser::OnAttributeNameReceived, this);
			_parser.AttributeValueReceived() += delegate(&DocumentParser::OnAttributeValueReceived, this);
			_parser.DataReceived() += delegate(&DocumentParser::OnDataReceived, this);
			_parser.EndElementReceived() += delegate(&DocumentParser::OnEndElementReceived, this);

			_stack.push(&_root);
		}

		/**
		 * The event used to notify listeners that a document has been successfully parsed.
		 *
		 * @return The event.
		 */
		DocumentParsedEvent& DocumentParsed() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _documentParsed;
		}

		/**
		 * Appends partial data to the parsing buffer so that it may be parsed.
		 *
		 * @param value The data to append.
		 * @return A reference to this parser.
		 */
		DocumentParser& Append(const std::string& value) { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			_parser.Append(value);
			return *this;
		}

		/**
		 * Gets the buffer of data that still needs to be parsed.
		 *
		 * @return The parsing buffer.
		 */
		const std::string& Buffer() const { StackTrace trace(__METHOD__, __FILE__, __LINE__);
			return _parser.Buffer();
		}

		/**
		 * Deletes this document parser.
		 */
		virtual ~DocumentParser() { StackTrace trace(__METHOD__, __FILE__, __LINE__);

		}
	};

	/**
	 * Performs unit testing on functions in this class to ensure expected operation.
	 */
	static void UnitTest() { StackTrace trace(__METHOD__, __FILE__, __LINE__);
		XmlElement xml("root");
		xml.Attribute("attr1").Value("12");
		xml.Element("test").Value("abc");
		xml.Element("test").Value("def");
		xml.Add("test").Value("ghi");

		assert(xml.ToString() == "<root attr1='12'><test>def</test><test>ghi</test></root>");
	}
};

XmlElement XmlElement::NullElement = XmlElement();
XmlAttribute XmlElement::NullAttribute = XmlAttribute();

}

#endif
