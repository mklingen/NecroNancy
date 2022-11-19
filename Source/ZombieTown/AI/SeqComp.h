#pragma once

#include <functional>
#include <vector>

namespace seqcomp
{
class SeqCompDomain
{
public:
	SeqCompDomain(std::function<bool(void)> check);
	std::function<bool(void)> DomainCheck;
};

class SeqCompController
{
public:
	SeqCompController() = default;
	SeqCompController(std::function<void(float)> tick);
	std::function<void(float)> Tick;
};

class SeqCompElement
{
public:
	SeqCompElement() = default;
	SeqCompElement(std::vector<SeqCompDomain> domains, SeqCompController controller);
	SeqCompElement(std::vector<std::function<bool(void)>> checks, SeqCompController controller);
	SeqCompElement(std::function<bool(void)> check, SeqCompController controller);
	SeqCompElement(std::function<bool(void)> check, std::function<void(float)> tick);
	bool TryTick(float dt);

	std::vector<SeqCompDomain> Domains;
	SeqCompController Controller;
};

class SeqComp
{
public:
	SeqComp() = default;
	void AddElement(SeqCompElement element);
	bool Tick(float dt);
	std::vector<SeqCompElement> Elements;
};
}  // namespace seqcomp