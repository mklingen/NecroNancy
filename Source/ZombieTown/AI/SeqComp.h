#pragma once

#include <functional>
#include <vector>

// Sequential Composition.
namespace seqcomp
{
	// Domain check in a sequential composition.
	class SeqCompDomain
	{
	public:
		SeqCompDomain(std::function<bool(void)> check);
		// Returns whether children of this subtree will tick.
		std::function<bool(void)> DomainCheck;
	};

	// Controller in a sequential composition.
	class SeqCompController
	{
	public:
		SeqCompController() = default;
		SeqCompController(std::function<void(float)> tick);
		std::function<void(float)> Tick;
	};

	// An element of a sequential composition, consisting of a domain and a controller.
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

	// A sequential composition tree.
	class SeqComp
	{
	public:
		SeqComp() = default;
		void AddElement(SeqCompElement element);
		bool Tick(float dt);
		std::vector<SeqCompElement> Elements;
	};
}  // namespace seqcomp