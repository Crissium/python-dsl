#include "dsl.h"

#include <algorithm>
#include <cctype>
#include <numeric>
#include <sstream>
#include <stdexcept>

std::string node::to_string() const
{
	if (!is_tag)
	{
		return text;
	}
	else
	{
		return std::accumulate(this->cbegin(),
							   this->cend(),
							   std::string(),
							   [](std::string &acc, const node &n)
							   { return acc + n.to_string(); });
	}
}

std::string node::traverse(const std::string &representation) const
{
	if (!is_tag)
	{
		return text;
	}
	else
	{
		std::string result = representation;
		result += "[" + tag_name + " " + tag_attrs + "]";
		for (auto const &n : *this)
		{
			result += n.traverse(representation);
		}
		return result;
	}
}

std::string node::to_xml() const
{
	if (!is_tag)
	{
		return text;
	}
	else
	{
		std::string result = "<" + tag_name;
		if (tag_attrs.empty())
		{
			result += ">";
		}
		else
		{
			result += " " + tag_attrs + ">";
		}
		for (const node &n : *this)
		{
			result += n.to_xml();
		}
		result += "</" + tag_name + ">";
		return result;
	}
}

const std::regex dom::re_brackets_blocks(R"(\{\{[^}]*\}\})");
const std::regex dom::re_trn_trs_tags(R"(\[(/?)(\!?)tr[ns]\])");
const std::regex dom::re_lang_open(R"(\[lang[^\]]*\])");
const std::regex dom::re_lang_close(R"(\[/lang\])");
const std::regex dom::re_com_tags(R"(\[(/?)com\])");
const std::regex dom::re_t_tags(R"(\[(/?)t\])");
const std::regex dom::re_asterisk_tags(R"(\[(/?)\*\])");

std::string dom::remove_unwanted_tags(const std::string &dsl_text)
{
	// Remove {{...}} blocks
	std::string text = std::regex_replace(dsl_text, re_brackets_blocks, "");

	// Remove trn/trs tags
	text = std::regex_replace(text, re_trn_trs_tags, "");

	// Remove lang tags
	// text = std::regex_replace(text, re_lang_open, "");
	text = std::regex_replace(text, re_lang_close, "");

	// Remove com tags
	text = std::regex_replace(text, re_com_tags, "");

	// Remove t tags
	text = std::regex_replace(text, re_t_tags, "");

	// Remove * tags
	text = std::regex_replace(text, re_asterisk_tags, "");

	return text;
}

std::string dom::preprocess(const std::string &dsl_text)
{
	std::istringstream text_stream(dsl_text);
	std::ostringstream result_stream;

	// For each line that begins with " [" but not " [m", or does not begin with a tag, wrap in "[m]...[/m]"
	std::string line;
	while (std::getline(text_stream, line))
	{
		if ((line.size() > 2 && line[0] == ' ' && line[1] == '[' && line[2] != 'm') || (line.size() > 1 && line[0] == ' ' && line[1] != '['))
		{
			result_stream << "[m]" << line << "[/m]\n";
		}
		else
		{
			result_stream << line << "\n";
		}
	}

	return result_stream.str();
}

bool dom::tag_is_m_n(const std::string &name_tag)
{
	return name_tag.size() == 2 && name_tag[0] == 'm' && std::isdigit(name_tag[1]);
}

bool dom::tag_is_m(const std::string &name_tag)
{
	return name_tag == "m" || tag_is_m_n(name_tag);
}

bool dom::check_m(const std::string &dst, const std::string &src)
{
	return src == "m" && tag_is_m_n(dst);
}

void dom::process_unsorted_parts(std::string &str, bool strip)
{
	int ref_count = 0;
	size_t start_pos = 0;

	for (size_t x = 0; x < str.size();)
	{
		char ch = str[x];
		if (ch == '\\')
		{
			x += 2;
			continue;
		}

		if (ch == '{')
		{
			++ref_count;

			if (!strip)
			{
				str.erase(x, 1);
				continue;
			}
			else if (ref_count == 1)
			{
				// First opening brace. Save this position, we will be erasing the
				// whole range when we encounter the last closing brace.
				start_pos = x;
			}
		}
		else if (ch == '}')
		{
			--ref_count;

			if (ref_count < 0)
			{
				ref_count = 0;
				str.erase(x, 1);
				continue;
			}

			if (!strip)
			{
				str.erase(x, 1);
				continue;
			}
			else if (!ref_count)
			{
				// The final closing brace -- we can erase the whole range now.
				str.erase(start_pos, x - start_pos + 1);
				x = start_pos;
				continue;
			}
		}

		++x;
	}

	if (strip && ref_count)
	{
		str.erase(start_pos);
	}
}

void dom::open_tag(const std::string &name, const std::string attrs, std::list<node *> &stack)
{
	std::list<node> nodes_to_reopen;

	if (tag_is_m(name))
	{
		// All tags above [m] tag will be closed and reopened after
		// to avoid break this tag by closing some other tag.

		while (!stack.empty())
		{
			nodes_to_reopen.emplace_back(std::move(*stack.back()));

			if (stack.back()->empty())
			{
				// Empty nodes are deleted since they're no use
				stack.pop_back();
				node *parent = stack.empty() ? &root : stack.back();
				parent->pop_back();
			}
			else
			{
				stack.pop_back();
			}
		}
	}

	// Add tag

	node n(name, attrs);

	if (stack.empty())
	{
		root.push_back(n);
		stack.push_back(&root.back());
	}
	else
	{
		stack.back()->push_back(n);
		stack.push_back(&stack.back()->back());
	}

	// Reopen tags if needed

	while (!nodes_to_reopen.empty())
	{
		if (stack.empty())
		{
			root.push_back(std::move(nodes_to_reopen.back()));
			stack.push_back(&root.back());
		}
		else
		{
			stack.back()->push_back(std::move(nodes_to_reopen.back()));
			stack.push_back(&stack.back()->back());
		}

		nodes_to_reopen.pop_back();
	}
}

void dom::close_tag(const std::string &name, std::list<node *> &stack)
{
	// Find the tag to be closed
	std::list<node *>::reverse_iterator n = std::find_if(
		stack.rbegin(),
		stack.rend(),
		[this, &name](const node *n)
		{ return n->tag_name == name || this->check_m(n->tag_name, name); });
	if (n != stack.rend())
	{
		// If there is a corresponding tag, close all tags above it,
		// then close the tag itself

		while (!stack.empty())
		{
			bool found = stack.back()->tag_name == name || check_m(stack.back()->tag_name, name);

			if (stack.back()->empty() && stack.back()->tag_name != "br")
			{
				// Empty nodes except [br] tag are deleted since they're no use
				stack.pop_back();
				node *parent = stack.empty() ? &root : stack.back();
				parent->pop_back();
			}
			else
			{
				stack.pop_back();
			}

			if (found)
			{
				break;
			}
		}
	}
}

void dom::next_char()
{
	if (!*string_pos)
		throw std::exception(); // Not exactly an exception, but we need to break out of the loop
	ch = *string_pos++;

	if (ch == '\\')
	{
		if (!*string_pos)
			throw std::exception(); // ditto

		ch = *string_pos++;
		escaped = true;
	}
	else if (ch == '[' && *string_pos == '[')
	{
		++string_pos;
		escaped = true;
	}
	else if (ch == ']' && *string_pos == ']')
	{
		++string_pos;
		escaped = true;
	}
	else
	{
		escaped = false;
	}

	if (ch == '\n' || ch == '\r') // '\r' should not appear, but just in case
	{
		line_start_pos = string_pos;
	}
}

dom::dom(const std::string &dsl_text)
	: root(std::string(), std::string())
{
	std::string cleaned_text = remove_unwanted_tags(dsl_text);
	cleaned_text = preprocess(cleaned_text);
	string_pos = cleaned_text.c_str();
	line_start_pos = string_pos;

	std::list<node *> stack; // currently opened tags

	node *text_node = nullptr; // current text node
	try
	{
		while (true)
		{
			next_char();

			if (ch == '[' && !escaped)
			{
				// Beginning of a tag
				bool is_closing;
				std::string name;
				std::string attrs;

				do
				{
					next_char();
				} while (std::isspace(ch)); // TODO: Unicode-aware?

				if (ch == '/' && !escaped)
				{
					// A closing tag
					is_closing = true;
					next_char();
				}
				else
				{
					is_closing = false;
				}

				// Read tag's name

				while ((ch != ']' || escaped) && !std::isspace(ch))
				{
					name.push_back(ch);
					next_char();
				}

				while (std::isspace(ch))
				{
					next_char();
				}

				// Read attrs

				while (ch != ']' || escaped)
				{
					attrs.push_back(ch);
					next_char();
				}

				// Add the tag, or close it

				if (text_node)
				{
					// Close the currently opened text node
					stack.pop_back();
					text_node = nullptr;
				}

				if (!is_closing)
				{
					if (tag_is_m(name))
					{
						close_tag("m", stack);
					}
					open_tag(name, attrs, stack);
					if (name == "br")
					{
						close_tag("br", stack);
					}
				}
				else
				{
					close_tag(name, stack);
				}
				continue;
			} // if ( ch == '[' )
			else if (ch == '<' && !escaped)
			{
				// Special case: the <<name>> link

				next_char();

				if (ch != '<' || escaped)
				{
					// OK, it's not it.
					--string_pos;

					if (escaped)
					{
						--string_pos;
						escaped = false;
					}
					ch = '<';
				}
				else
				{
					// Get the link's body
					do
					{
						next_char();
					} while (std::isspace(ch));

					std::string link_to, link_text;

					for (;; next_char())
					{
						// Is it the end?
						if (ch == '>' && !escaped)
						{
							next_char();

							if (ch == '>' && !escaped)
								break;
							else
							{
								link_to.push_back('>');
								link_to.push_back(ch);

								link_text.push_back('>');
								if (escaped)
									link_text.push_back('\\');
								link_text.push_back(ch);
							}
						}
						else
						{
							link_to.push_back(ch);
							if (escaped)
								link_text.push_back('\\');
							link_text.push_back(ch);
						}
					}

					// Add the corresponing node

					if (text_node)
					{
						// Close the currently opened text node
						stack.pop_back();
						text_node = nullptr;
					}

					trim(link_text);
					process_unsorted_parts(link_text, true);
					dom node_dom(link_text);
					node link("ref", std::string());
					for (auto &n : node_dom.root)
					{
						link.push_back(std::move(n));
					}

					if (stack.empty())
					{
						root.push_back(link);
					}
					else
					{
						stack.back()->push_back(link);
					}
					continue;
				}
			} // if ( ch == '<' )

			// If we're here, we've got a normal symbol, to be saved as text.

			// If there's currently no text node, open one
			if (!text_node)
			{
				node text = node(std::string());

				if (stack.empty())
				{
					root.push_back(text);
					stack.push_back(&root.back());
				}
				else
				{
					stack.back()->push_back(text);
					stack.push_back(&stack.back()->back());
				}

				text_node = stack.back();
			}

			if (escaped && ch == ' ')
			{
				// ch = '\xa0'; // Escaped spaces turn into non-breakable ones in Lingvo
				// The statement commented above is from GoldenDict, which turns spaces into strange UTF-8 'REPLACEMENT CHARACTER'
				// I don't know why it's needed, so I'm commenting it out for now.
			}

			text_node->text.push_back(ch);
		}
	}
	catch (std::exception)
	{
	}

	if (text_node)
	{
		stack.pop_back();
	}
}
