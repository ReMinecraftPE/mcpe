#include <typeinfo>
#include "thirdparty/glm/glm.hpp"
#include "thirdparty/glm/ext/matrix_transform.hpp"
#include "thirdparty/glm/ext/matrix_clip_space.hpp"
#include "MatrixStack.hpp"
#include "common/Logger.hpp"

Matrix Matrix::IDENTITY = Matrix(1.0f);

MatrixStack MatrixStack::View = MatrixStack();
MatrixStack MatrixStack::World = MatrixStack();
MatrixStack MatrixStack::Projection = MatrixStack();

Matrix::Matrix()
{
    _m = glm::mat4();
}

Matrix::Matrix(float s)
{
    _m = glm::mat4(s);
}

Matrix::Matrix(const glm::mat4& v)
{
    _m = v;
}

void Matrix::rotate(float angle, const Vec3& axis)
{
    _m = glm::rotate(_m, glm::radians(angle), glm::normalize(glm::vec3(axis.x, axis.y, axis.z)));
}

void Matrix::scale(const Vec3& s)
{
    _m = glm::scale(_m, glm::vec3(s.x, s.y, s.z));
}

void Matrix::setOrtho(float left, float right, float bottom, float top, float Znear, float Zfar)
{
    _m = glm::ortho(left, right, bottom, top, Znear, Zfar);
}

void Matrix::setPerspective(float fov, float aspect, float Znear, float Zfar)
{
    _m = glm::perspective(glm::radians(fov), aspect, Znear, Zfar);
}

void Matrix::transform3(Vec3& outVec, float& outW)
{
    glm::vec4 temp_vec(glm::vec3(outVec.x, outVec.y, outVec.z), outW);

    glm::vec4 result = _m * temp_vec;
    glm::vec3 result3(result);

    outVec = Vec3(result.x, result.y, result.z);
    outW = result.w;
}

void Matrix::translate(const Vec3& t)
{
    _m = glm::translate(_m, glm::vec3(t.x, t.y, t.z));
}

MatrixStack::MatrixStack()
{
    m_bIsDirty = true;
    _pushIdentity();
}

MatrixStack::Ref* MatrixStack::push()
{
    m_bIsDirty = true;
    return new Ref(*this, _push());
}

MatrixStack::Ref* MatrixStack::pushIdentity()
{
    m_bIsDirty = true;
    return new Ref(*this, _pushIdentity());
}

Matrix& MatrixStack::_push()
{
    m_stack.push(Matrix());
    return m_stack.top();
}

Matrix& MatrixStack::_pushIdentity()
{
    m_stack.push(Matrix::IDENTITY);
    return m_stack.top();
}

const Matrix& MatrixStack::top() const
{
    return m_stack.top();
}

const Matrix& MatrixStack::getTop()
{
    m_bIsDirty = true;
    return top();
}

void MatrixStack::pop()
{
    m_bIsDirty = true;

    if (empty())
    {
        LOG_E("Bad stack usage");
        throw std::bad_cast();
    }

    m_stack.pop();
}

MatrixStack::Ref::Ref()
{
	m_mtxStack = nullptr;
	m_matrix = nullptr;
}

MatrixStack::Ref::Ref(MatrixStack& mtxStk, Matrix& mtx)
{
	m_mtxStack = &mtxStk;
	m_matrix = &mtx;
}

MatrixStack::Ref::Ref(Ref&& other)
{
	_move(other);
}

MatrixStack::Ref::~Ref()
{
    release();
}

void MatrixStack::Ref::_move(MatrixStack::Ref& other)
{
    if (this == &other)
        return;

    if (m_matrix || m_mtxStack)
    {
        LOG_E("It doesn't really make sense to pop here, so can't release");
        throw std::bad_cast();
    }

    this->m_mtxStack = other.m_mtxStack;
    this->m_matrix = other.m_matrix;
    other.m_mtxStack = nullptr;
}

void MatrixStack::Ref::release()
{
    if (m_mtxStack)
        m_mtxStack->pop();
    m_matrix = nullptr;
    m_mtxStack = nullptr;
}

Matrix* MatrixStack::Ref::operator*()
{
	if (!m_matrix)
	{
		LOG_E("Dereferencing a null reference");
		throw std::bad_cast();
	}
	m_mtxStack->m_bIsDirty = true;
	return m_matrix;
}

MatrixStack::Ref& MatrixStack::Ref::operator=(const Matrix& value)
{
    *(this->m_matrix) = value;
    return *this;
}

void MatrixStack::Ref::operator=(Ref&& other)
{
    _move(other);
}