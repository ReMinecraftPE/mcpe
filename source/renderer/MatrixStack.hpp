#pragma once

#include <stack>
#define GLM_FORCE_RADIANS
#include "thirdparty/glm/glm.hpp"
#include "compat/LegacyCPP.hpp"
#include "world/phys/Vec3.hpp"

enum MatrixType
{
    MATRIX_VIEW,
    MATRIX_PROJECTION
};

class Matrix
{
public:
    static Matrix EMPTY;
    static Matrix IDENTITY;
    
public:
    glm::mat4 _m;

public:
	Matrix();         // create an empty matrix
	Matrix(float s);  // create an identity matrix
    Matrix(const glm::mat4& v);

    void rotate(float angle, const Vec3& axis);
	void scale(const Vec3& s);
	void setOrtho(float left, float right, float bottom, float top, float Znear, float Zfar);
	void setPerspective(float fov, float aspect, float Znear, float Zfar);
	void transform3(Vec3& outVec, float& outW);
	void translate(const Vec3& t);
    const float* ptr() const;
    float* getPtr();
    
    Matrix operator*(const Matrix& other) const;
};

class MatrixStack
{
public:
    class Ref;

public:
    static MatrixStack View;
    static MatrixStack World;
    static MatrixStack Projection;

private:
    std::stack<Matrix> m_stack;
    bool m_bIsDirty;

public:
    MatrixStack();
    
private:
    Matrix& _push();
    Matrix& _pushIdentity();

public:
    const Matrix& top() const;
    Matrix& getTop();
    bool empty() const { return m_stack.empty(); }
    bool isDirty() const { return m_bIsDirty; }
    void makeClean() { m_bIsDirty = false; };
    void pop();
    size_t size() const { return m_stack.size(); }
    Ref push();
    Ref pushIdentity();

    class Ref
    {
    private:
        MatrixStack* m_pStack;
        Matrix* m_pMatrix;

    public:
        Ref();
        Ref(MatrixStack& mtxStk, Matrix& mtx);
        MC_CTOR_MOVE(Ref);
        ~Ref();
        
    private:
        void _move(Ref& other);

    public:
        void release();

        const MatrixStack* stack() const { return m_pStack; }
        const Matrix* matrix() const { return m_pMatrix; }

        operator Matrix&() { return *m_pMatrix; }
        Matrix* operator->();
        Matrix* operator*();
        Ref& operator=(const Matrix& value);
        MC_FUNC_MOVE(Ref);
    };
};