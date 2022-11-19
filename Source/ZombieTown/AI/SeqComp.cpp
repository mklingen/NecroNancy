#include "SeqComp.h"

namespace seqcomp
{

SeqCompDomain::SeqCompDomain(std::function<bool(void)> check) :
	DomainCheck(check) {

}

SeqCompController::SeqCompController(std::function<void(float)> tick) :
	Tick(tick) {

}

SeqCompElement::SeqCompElement(std::vector<SeqCompDomain> domains, SeqCompController controller) :
	Domains(domains), Controller(controller)
{

}

SeqCompElement::SeqCompElement(std::vector<std::function<bool(void)>> checks, SeqCompController controller) :
	Controller(controller)
{
	for (const auto& check : checks)
	{
		Domains.push_back(SeqCompDomain(check));
	}
}

SeqCompElement::SeqCompElement(std::function<bool(void)> check, SeqCompController controller) :
	Controller(controller)
{
	Domains.push_back(SeqCompDomain(check));
}

SeqCompElement::SeqCompElement(std::function<bool(void)> check, std::function<void(float)> tick) :
	Controller(tick)
{
	Domains.push_back(SeqCompDomain(check));
}

bool SeqCompElement::TryTick(float dt)
{
	for (SeqCompDomain& domain : Domains)
	{
		if (!domain.DomainCheck()) {
			return false;
		}
	}
	Controller.Tick(dt);
	return true;
}

void SeqComp::AddElement(SeqCompElement element) 
{
	Elements.push_back(element);
}

bool SeqComp::Tick(float dt)
{
	for (SeqCompElement& element : Elements)
	{
		if (element.TryTick(dt))
		{
			return true;
		}
	}
	return false;
}
}  // namespace seqcomp