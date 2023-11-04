#pragma once

#include <array>
#include <list>
#include <string>
#include <regex>
#include <vector>

void ltrim(std::string &s);
void rtrim(std::string &s);
void trim(std::string &s);
std::string html_escape(const std::string &s);
bool endswith(const std::string &str, const std::string &suffix);

struct node : public std::list<node>
{
	bool is_tag; // false if it's a text node (leaf)

	// For tag nodes
	std::string tag_name;
	std::string tag_attrs;

	// For text nodes
	std::string text;

	node(std::string tag_name, std::string tag_attrs) : is_tag(true),
														tag_name(tag_name),
														tag_attrs(tag_attrs) {}

	node(std::string text) : is_tag(false),
							 text(text) {}

	/**
	 * @brief Recursively converts the node and its children to a string.
	 * @return The string representation of the node and its children.
	 */
	std::string to_string() const;

	/**
	 * @brief Traverse root-first, depth-first.
	 * @return Tag/text.
	 */
	std::string traverse(const std::string &representation) const;

	/**
	 * @brief Recursively converts the node and its children to a string in XML.
	 * @return The string representation of the node and its children in XML.
	 */
	std::string to_xml() const;
};

class dom
{
private:
	static const std::regex re_brackets_blocks;
	static const std::regex re_trn_trs_tags;
	static const std::regex re_lang_open;
	static const std::regex re_lang_close;
	static const std::regex re_com_tags;
	static const std::regex re_t_tags;		  // transcription
	static const std::regex re_asterisk_tags; // secondary/optional
	static std::string remove_unwanted_tags(const std::string &dsl_text);

	static std::string preprocess(const std::string &dsl_text);

	static bool tag_is_m_n(const std::string &name_tag);
	static bool tag_is_m(const std::string &name_tag);
	static bool check_m(const std::string &dst, const std::string &src);

	static void process_unsorted_parts(std::string &str, bool strip);

	char const *string_pos;
	char const *line_start_pos;

	char ch;
	bool escaped;

	void open_tag(const std::string &name, const std::string attrs, std::list<node *> &stack);
	void close_tag(const std::string &name, std::list<node *> &stack);

	void next_char();

public:
	node root;

	dom(const std::string &dsl_text);
};

class builder
{
private:
	static bool is_image(const std::string &filename);
	static bool is_audio(const std::string &filename);
	static bool is_video(const std::string &filename);

	static std::string get_node_link(const node &n);

	const std::string url_cache_root;
	const std::string url_lookup_root;

	bool audio_found;

	void write_children(const node &n);

	void node_to_html(const node &n);

	void write_text(const node &n);
	void write_b(const node &n);
	void write_i(const node &n);
	void write_u(const node &n); // [u] and [']
	void write_sub(const node &n);
	void write_sup(const node &n);
	void write_colour(const node &n);
	void write_m(const node &n);
	void write_m_n(const node &n);
	void write_example(const node &n);
	void write_media(const node &n);
	void write_ref(const node &n);
	void write_url(const node &n);
	void write_p(const node &n); // abbr
	void write_br(const node &n);
	void write_unknown(const node &n);

public:
	std::string html;
	std::vector<std::string> resources_name;

	builder(const std::string &name_dict);

	void build(const node &root);
};
