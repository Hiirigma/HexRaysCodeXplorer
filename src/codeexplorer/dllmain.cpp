// dllmain.cpp : Defines the entry point for the DLL application.
#include "stdafx.h"

#include <pybind11/pybind11.h>
namespace py = pybind11;

#include <pybind11/stl.h>

#include <ReconstructableType.h>

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

std::list<std::string> getKnownTypes() {
	std::list<std::string> result;
	result.clear();
	std::map<std::string, ReconstructableType*>::iterator it= g_ReconstractedTypes.begin();
	while (it != g_ReconstractedTypes.end()) {
		result.push_back(it->first);
		it++;
	}
	return result;
}





#ifdef __EA64__
PYBIND11_MODULE(codeexplorer64, m) {
#else
PYBIND11_MODULE(codeexplorer32, m) {
#endif
	py::class_<ReconstructableType> reconstructableType(m, "ReconstructableType");
	reconstructableType.def_static("getReconstructableType", &ReconstructableType::getReconstructableType)
		.def_readonly("name", &ReconstructableType::name)
		.def_readonly("typeId", &ReconstructableType::typeId)
		.def_readonly("maxSize", &ReconstructableType::maxSize)
		.def("SetMemberName", &ReconstructableType::SetMemberName)
		.def("SetMemberNameUpcast", &ReconstructableType::SetMemberNameUpcast)
		.def("SetMemberType", &ReconstructableType::SetMemberType)
		.def("SetMemberTypeUpcast", &ReconstructableType::SetMemberTypeUpcast)
		.def("AddMember", &ReconstructableType::AddMember)
		.def("AddMemberUpcast", &ReconstructableType::AddMemberUpcast)
		.def("AddDerivedMember", &ReconstructableType::AddDerivedMember)
		.def("getSize", &ReconstructableType::getSize)
		.def("findMemberByOffset", &ReconstructableType::findMemberByOffset)
		.def("CopyMembersToOther", &ReconstructableType::CopyMembersToOther)
		.def("UndefMembers", &ReconstructableType::UndefMembers)
		.def("AddSubType", &ReconstructableType::AddSubType)
		.def("SyncTypeInfo", &ReconstructableType::SyncTypeInfo)
		.def("SetMaxSize", &ReconstructableType::SetMaxSize)
		.def("getParents", &ReconstructableType::getParents)
		.def("getChildren", &ReconstructableType::getChildren)
		.def("__repr__", 
			[](const ReconstructableType &a) {
				return "<codeexplorer.ReconstructableType named '" + a.name + "'>";
			}
	);

	py::class_<ReconstructableTypeVtable, ReconstructableType> reconstructableTypeVtable(m,  "ReconstructableTypeVtable");
	reconstructableTypeVtable.def_static("getReconstructableTypeVtable", &ReconstructableTypeVtable::getReconstructableTypeVtable)
		.def_readonly("vtable_address", &ReconstructableTypeVtable::vtable_address)
		.def("to_ea", &ReconstructableTypeVtable::to_ea);

	py::enum_< MemberTypeKind>(m, "MemberTypeKind")
		.value("MemberType_Unknown", MemberTypeKind::MemberType_Unknown)
		.value("MemberType_Reconsrtucted", MemberTypeKind::MemberType_Reconsrtucted)
		.value("MemberType_Pointer", MemberTypeKind::MemberType_Pointer)
		.value("MemberType_IDAGate", MemberTypeKind::MemberType_IDAGate)
		.export_values();


	py::class_< ReconstructableMember>(m, "ReconstructableMember")
		.def(py::init<>())
		.def_readwrite("offset", &ReconstructableMember::offset)
		.def_readwrite("name", &ReconstructableMember::name)
		.def_readwrite("memberType", &ReconstructableMember::memberType);

	// abstract class
	py::class_< ReconstructedMemberType>(m, "ReconstructedMemberType")
		.def("getSize", &ReconstructedMemberType::getSize)
		.def("getTypeString", &ReconstructedMemberType::getTypeString)
		.def("getKind", &ReconstructedMemberType::getKind);


	py::class_<ReconstructedMemberReType, ReconstructedMemberType>(m, "ReconstructedMemberReType")
		.def(py::init<ReconstructableType *>())
		.def_readonly("reType", &ReconstructedMemberReType::reType);

	
	py::class_<MemberTypeIDATypeInfoGate, ReconstructedMemberType>(m, "MemberTypeIDATypeInfoGate")
		.def(py::init<tinfo_t>())
		.def_readonly("info", &MemberTypeIDATypeInfoGate::info);

	py::class_<MemberTypePointer, ReconstructedMemberType>(m, "MemberTypePointer")
		.def(py::init<std::string>())
		.def_readonly("pointedType", &MemberTypePointer::pointedType);

	m.doc() = "codeexplorer API plugin"; // optional module docstring
	m.def("getKnownTypes", &getKnownTypes);

	m.def("re_types_form_init", &re_types_form_init);
}