#include "src/dsl.h"

#include <iostream>

int main()
{
	dom tree("[m]word[c]ss[/c],[s]video.mp3[/s][s]a.mp4[/s][s]s.jpg[/s]");
	builder b("test");
	std::string html = b.get_html(tree.root);
	std::cout << html << '\n';
	for (auto &s : b.resources_name)
	{
		std::cout << s << '\n';
	}
}