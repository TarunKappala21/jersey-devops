import { useState } from 'react'
import { Link, useNavigate } from 'react-router-dom'
import { login } from '../api/client'
import { useAuth } from '../context/AuthContext'
import toast from 'react-hot-toast'
import { LogIn } from 'lucide-react'

export default function Login() {
  const [form, setForm]       = useState({ username: '', password: '' })
  const [loading, setLoading] = useState(false)
  const { signIn }            = useAuth()
  const navigate              = useNavigate()

  const handleSubmit = async e => {
    e.preventDefault()
    setLoading(true)
    try {
      const res  = await login(form.username, form.password)
      const data = res.data.data
      signIn(data, data.token)
      toast.success(`Welcome back, ${data.name}!`)
      navigate(data.role === 'admin' ? '/admin' : '/')
    } catch {
      toast.error('Invalid username or password')
    } finally {
      setLoading(false)
    }
  }

  return (
    <div style={{
      minHeight: '100vh', display: 'flex',
      alignItems: 'center', justifyContent: 'center',
      background: 'radial-gradient(ellipse at 60% 40%, rgba(181,255,77,0.04) 0%, transparent 60%)',
    }}>
      <div style={{ width: '100%', maxWidth: 400, padding: '0 24px' }}>

        {/* Logo */}
        <div style={{ textAlign: 'center', marginBottom: 40 }}>
          <h1 style={{ fontSize: '3rem', lineHeight: 1 }}>
            JERSEY<span className="accent">VAULT</span>
          </h1>
          <p className="muted" style={{ marginTop: 8, fontSize: '0.9rem' }}>
            Sign in to your account
          </p>
        </div>

        {/* Form */}
        <div className="card fade-up" style={{ padding: 32 }}>
          <form onSubmit={handleSubmit} style={{ display: 'flex', flexDirection: 'column', gap: 16 }}>
            <div>
              <label style={{ fontSize: '0.8rem', color: 'var(--text-secondary)', display: 'block', marginBottom: 6 }}>
                Username
              </label>
              <input
                className="input"
                placeholder="tarun"
                value={form.username}
                onChange={e => setForm(f => ({ ...f, username: e.target.value }))}
                required
              />
            </div>

            <div>
              <label style={{ fontSize: '0.8rem', color: 'var(--text-secondary)', display: 'block', marginBottom: 6 }}>
                Password
              </label>
              <input
                className="input"
                type="password"
                placeholder="••••••••"
                value={form.password}
                onChange={e => setForm(f => ({ ...f, password: e.target.value }))}
                required
              />
            </div>

            <button className="btn btn-primary" type="submit" disabled={loading}
              style={{ width: '100%', justifyContent: 'center', marginTop: 8, padding: '12px' }}>
              <LogIn size={16} />
              {loading ? 'Signing in...' : 'Sign In'}
            </button>
          </form>

          <hr className="divider" />

          {/* Demo accounts */}
          <div style={{ fontSize: '0.78rem', color: 'var(--text-muted)' }}>
            <p style={{ marginBottom: 8, color: 'var(--text-secondary)' }}>Demo accounts:</p>
            {[
              { user: 'tarun',  pass: 'tarun123',  role: 'Customer' },
              { user: 'admin',  pass: 'admin123',  role: 'Admin' },
            ].map(a => (
              <div key={a.user}
                onClick={() => setForm({ username: a.user, password: a.pass })}
                style={{
                  display: 'flex', justifyContent: 'space-between',
                  padding: '6px 10px', marginBottom: 4,
                  background: 'var(--bg-raised)', borderRadius: 'var(--radius-sm)',
                  cursor: 'pointer', transition: 'var(--transition)',
                }}
                onMouseEnter={e => e.currentTarget.style.borderColor = 'var(--accent)'}
              >
                <span className="mono">{a.user} / {a.pass}</span>
                <span className="accent">{a.role}</span>
              </div>
            ))}
          </div>
        </div>

        <p style={{ textAlign: 'center', marginTop: 20, fontSize: '0.85rem', color: 'var(--text-secondary)' }}>
          No account? <Link to="/register" style={{ color: 'var(--accent)' }}>Register</Link>
        </p>
      </div>
    </div>
  )
}
